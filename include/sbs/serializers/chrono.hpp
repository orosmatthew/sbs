#pragma once

#include <sbs/sbs.hpp>

#include <chrono>

namespace sbs {

template <class Tick, class TickSerializer = DefaultSerializer<Tick>, class Period = std::ratio<1>>
    requires(sbs::Serializer<TickSerializer, Tick> && std::copyable<Tick> && std::is_default_constructible_v<Tick>)
struct ChronoDurationSerializer {
    void operator()(Archive& ar, std::chrono::duration<Tick, Period>& duration) const
    {
        if (ar.serializing()) {
            const Tick ticks = duration.count();
            ar.archive_copy<TickSerializer>(ticks);
        } else {
            auto ticks = Tick();
            ar.archive<TickSerializer>(ticks);
            duration = std::chrono::duration<Tick, Period> { ticks };
        }
    }
};

template <class Clock, class Duration = Clock::duration, class DurationSerializer = DefaultSerializer<Duration>>
    requires(
        sbs::Serializer<DurationSerializer, Duration> && std::copyable<Duration>
        && std::is_default_constructible_v<Duration>)
struct ChronoTimePointSerializer {
    void operator()(Archive& ar, std::chrono::time_point<Clock, Duration>& time_point) const
    {
        if (ar.serializing()) {
            const Duration ticks = time_point.time_since_epoch();
            ar.archive_copy<DurationSerializer>(ticks);
        } else {
            auto duration = Duration();
            ar.archive<DurationSerializer>(duration);
            time_point = std::chrono::time_point<Clock, Duration> { duration };
        }
    }
};

template <class Tick, class TickSerializer = DefaultSerializer<Tick>, class Period = std::ratio<1>>
    requires(sbs::Serializer<TickSerializer, Tick>)
void serialize(Archive& ar, std::chrono::duration<Tick, Period>& duration)
{
    ChronoDurationSerializer<Tick, TickSerializer, Period>()(ar, duration);
}

template <class Clock, class Duration = Clock::duration, class DurationSerializer = DefaultSerializer<Duration>>
    requires(sbs::Serializer<DurationSerializer, Duration>)
void serialize(Archive& ar, std::chrono::time_point<Clock, Duration>& time_point)
{
    ChronoTimePointSerializer<Clock, Duration, DurationSerializer>()(ar, time_point);
}

}