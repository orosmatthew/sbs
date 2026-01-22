#ifndef SBS_SERIALIZERS_CHRONO_HPP
#define SBS_SERIALIZERS_CHRONO_HPP

#include <sbs/sbs.hpp>

#include <chrono>
#include <cstdint>

namespace sbs {

template <class Tick, class TickSerializer = DefaultSerializer<Tick>, class Period = std::ratio<1>>
    requires(sbs::Serializer<TickSerializer, Tick> && std::copyable<Tick> && std::is_default_constructible_v<Tick>)
struct ChronoDurationSerializer {
    void operator()(Archive& ar, std::chrono::duration<Tick, Period>& duration) const
    {
        if (ar.serializing()) {
            Tick ticks = duration.count();
            ar.archive<TickSerializer>(ticks);
        } else {
            auto ticks = Tick();
            ar.archive<TickSerializer>(ticks);
            duration = std::chrono::duration<Tick, Period> { ticks };
        }
    }
};

template <
    class Clock,
    class Duration = std::chrono::duration<int64_t, std::nano>,
    class DurationSerializer = DefaultSerializer<Duration>>
    requires(
        sbs::Serializer<DurationSerializer, Duration> && std::copyable<Duration>
        && std::is_default_constructible_v<Duration>)
struct ChronoTimePointSerializer {
    void operator()(Archive& ar, std::chrono::time_point<Clock>& time_point) const
    {
        if (ar.serializing()) {
            Duration duration = std::chrono::duration_cast<Duration>(time_point.time_since_epoch());
            ar.archive<DurationSerializer>(duration);
        } else {
            auto duration = Duration();
            ar.archive<DurationSerializer>(duration);
            time_point
                = std::chrono::time_point<Clock> { std::chrono::duration_cast<typename Clock::duration>(duration) };
        }
    }
};

template <class Tick, class TickSerializer = DefaultSerializer<Tick>, class Period = std::ratio<1>>
    requires(sbs::Serializer<TickSerializer, Tick>)
void serialize(Archive& ar, std::chrono::duration<Tick, Period>& duration)
{
    ChronoDurationSerializer<Tick, TickSerializer, Period>()(ar, duration);
}

template <class Clock>
void serialize(Archive& ar, std::chrono::time_point<Clock>& time_point)
{
    ChronoTimePointSerializer<Clock>()(ar, time_point);
}

}

#endif // SBS_SERIALIZERS_CHRONO_HPP