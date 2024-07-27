#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

using namespace spdlog;

void init_logger()
{
    auto stdout_sink = std::make_shared<sinks::stdout_color_sink_mt>();

    size_t max_size_bytes = 1 * 1024 * 1024;
    size_t max_files = 3;
    auto rotating_sink = std::make_shared<sinks::rotating_file_sink_mt>("logs/breaq-reminder.log",
                                                                        max_size_bytes,
                                                                        max_files);

    std::vector<sink_ptr> sinks{stdout_sink, rotating_sink};
    spdlog::logger lgr("multi_sink", {stdout_sink, rotating_sink});
    // q_size means the number of items in the queue, not the size in byte of the
    // queue
    /*
    init_thread_pool(1024 * 8, 1);
    auto logger = std::make_shared<async_logger>("qrammer_logger",
                                                 sinks.begin(),
                                                 sinks.end(),
                                                 thread_pool(),
                                                 async_overflow_policy::overrun_oldest);
    */
    spdlog::set_default_logger(std::make_shared<logger>(lgr));
    spdlog::set_pattern("%Y-%m-%dT%T.%f%z | %5t | %40! | %8l | %v");
}
