#include <sstream>
#include <bluetooth/bluetooth.h>
#include "config_tree.h"
#include "writer.h"
#include "util.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Writer::Writer(BLE &_ble, ConfigTree &_config, ExpectedResponses &_expected)
    : ble(_ble), config(_config), expected(_expected),
      next_heartbeat(std::time(nullptr)+10),
      running(true),
      writer_t ([&]() {
    writer_thread();
}),
heartbeat_t([&]() {
    heartbeat_thread();
}) {
}
//---------------------------------------------------------------------------
Writer::~Writer() {
    running = false;
    q.force_quit();
    cv.notify_one();
    writer_t   .join();
    heartbeat_t.join();
}
//---------------------------------------------------------------------------
// Heartbeat thread sends PCB_VERSION command after 10 seconds since last
// write. Required to keep connection alive.
void Writer::heartbeat_thread() {
    try {
        std::mutex cv_m;
        while (running) {
            auto t = next_heartbeat.load();
            {
                std::unique_lock<std::mutex> guard(cv_m);
                auto now = std::time(nullptr);
                if (now < t) {
                    cv.wait_for(guard, std::chrono::seconds(t - now));
                }
            }
            if (!running) {
                break;
            }
            if (t<next_heartbeat.load()) {
                continue;
            }
            cmd("PCB_VERSION");
            next_heartbeat = std::time(nullptr) + 10;
        }
    } catch (std::exception &ex) {
        StdOutExclusiveAccess guard;
        std::cerr << "heartbeat thread: " << ex.what() << std::endl;
    }
}
//---------------------------------------------------------------------------
void Writer::writer_thread() {
    try {
        while (running) {
            auto e = q.pop();
            expected.push(e.cmd, std::move(e.p));
            next_heartbeat = std::time(nullptr) + 10;
            ble.write(e.buf);
        }
    } catch (ForceQuit &) {
    } catch (std::exception &ex) {
        StdOutExclusiveAccess guard;
        std::cerr << "writer thread: " << ex.what() << std::endl;
    }
}
//---------------------------------------------------------------------------
std::future<std::string> Writer::cmd(std::string cmd) {
    std::string buf;
    std::string::size_type idx = cmd.find(' ');
    std::string arg;
    if (idx != cmd.npos) {
        arg = cmd.substr(idx+1);
        cmd = cmd.substr(0, idx);
    }
    auto c = config.tree_by_name.find(cmd);
    if (c==config.tree_by_name.end()) {
        throw std::runtime_error("Writer: "+cmd+": no such command");
    }
    if (idx == cmd.npos) {
        buf.push_back(c->second->id);
    } else {
        buf.push_back(c->second->id | 0x80);

        if (c->second->type==U8 || c->second->type==U16 || c->second->type==U32) {
            int i = int(std::stod(arg));
            if (c->second->type==U8 && i>=0 && i<=0xff) {
                buf.push_back(i);
            } else if (c->second->type==U16 && i>=0 && i<=0xffff) {
                buf.append(2, '\0');
                bt_put_le16(i, &buf[1]);
            } else if (c->second->type==U32) {
                buf.append(4, '\0');
                bt_put_le32(i, &buf[1]);
            } else {
                throw std::runtime_error("Writer: "+cmd+" "+arg+": invalid arg");
            }
        } else if (c->second->type==FLT) {
            float f = std::stod(arg);
            buf.append(4, '\0');
            memcpy(&buf[1], &f, 4);
        } else if (c->second->type==STR) {
            uint16_t len = arg.length();
            buf.append(2 + len, '\0');
            bt_put_le16(len, &buf[1]);
            memcpy(&buf[3], &arg[0], len);
        } else if (c->second->type==CHOOSER) {
            unsigned jdx;
            for (jdx=0; jdx<c->second->chooser_values.size(); ++jdx) {
                if (c->second->chooser_values[jdx].value==arg) {
                    break;
                }
            }
            if (jdx==c->second->chooser_values.size()) {
                std::ostringstream os;
                os << "Writer: " << cmd << " " << arg << ": invalid arg, "
                   "available values {";
                for (jdx=0; jdx<c->second->chooser_values.size(); ++jdx) {
                    if (jdx) {
                        os << ", ";
                    }
                    os << c->second->chooser_values[jdx].value;
                }
                os << "}";
                throw std::runtime_error(os.str());
            }
            c->second->chooser_idx = jdx;
            buf.push_back(jdx);
        } else {
            throw std::runtime_error("Writer: "+cmd+": unexpected arg");
        }
    }
    if (idx != cmd.npos) {
        c->second->value = arg;
        config.update_range_i(cmd);
        std::string cmd2;
        if (cmd == "CH1:MAPPING") {
            cmd2 = "CH1:RANGE_I";
        } else if (cmd == "CH2:MAPPING") {
            cmd2 = "CH2:RANGE_I";
        }
        if (cmd2.size()) {
            std::string buf2;
            buf2.push_back(config.tree_by_name[cmd2]->id);
            buf2.push_back(0);
            q.push({ cmd2, buf2, std::move(std::promise<std::string>()) });
        }
    }
    std::promise<std::string> p;
    std::future<std::string> f = p.get_future();
    q.push({ cmd, buf, std::move(p) });
    return f;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
