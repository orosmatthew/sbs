#pragma once

#include <sbs/sbs.hpp>

#include <chrono>

namespace sbs {

template <class Tick, class TickSerializer = DefaultSerializer<Tick>, class Period = std::ratio<1>>
    requires(sbs::Serializer<TickSerializer, Tick> && std::copyable<Tick> && std::is_default_constructible_v<Tick>)
struct ChronoDurationSerializer {
    void operator()(std::chrono::duration<Tick, Period>& duration, Archive& ar) const
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
    void operator()(std::chrono::time_point<Clock, Duration>& time_point, Archive& ar) const
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
void serialize(std::chrono::duration<Tick, Period>& duration, Archive& ar)
{
    ChronoDurationSerializer<Tick, TickSerializer, Period>()(duration, ar);
}

template <class Clock, class Duration = Clock::duration, class DurationSerializer = DefaultSerializer<Duration>>
    requires(sbs::Serializer<DurationSerializer, Duration>)
void serialize(std::chrono::time_point<Clock, Duration>& time_point, Archive& ar)
{
    ChronoTimePointSerializer<Clock, Duration, DurationSerializer>()(time_point, ar);
}

}