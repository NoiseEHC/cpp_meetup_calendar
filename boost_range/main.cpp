#include "stdafx.h"
#include "day_iterator.h"
#include "group_by.h"
#include <boost/range/any_range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>

auto dates_in_year(int year)
{
    return boost::iterator_range<day_iterator>(
        day_iterator{date{date::year_type(year), greg::Jan, 1}},
        day_iterator{date{date::year_type(year + 1), greg::Jan, 1}});
}

auto by_month()
{
    return grouped_by([](date const& a, date const& b)
    {
        return a.month() == b.month();
    });
}

auto by_week()
{
    return grouped_by([](date const& a, date const& b)
    {
        return a.week_number() == b.week_number();
    });
}

auto month_by_week()
{
    return boost::adaptors::transformed([](auto month)
    {
        return month | by_week();
    });
}

struct is_sunday
{
    bool operator()(date day) const
    {
        return day.day_of_week() == 0;
    }
};

int main()
{
    // const does not compile because of const-ness in group_by implementation...
    auto year = dates_in_year(2018);

    auto months = year | by_month() | month_by_week();
    for (auto const& month : months) // cannot be non-const reference!!!
    {
        std::cout << "===== " << month.front().front().month() << std::endl;
        for (auto const& week : month)
        {
            for (auto const& day : week | boost::adaptors::transformed([](date d) { return d.day(); }))
                std::cout << day << " ";
            std::cout << std::endl;
        }
    }

    //// This one does not compile!!!
    //auto rng = year;
    //bool const switchOnlyMondays = true;
    //if (switchOnlyMondays)
    //    rng = rng | boost::adaptors::filtered([](date d) { return d.day_of_week() == 0; });
    //for (auto const& day : rng)
    //    std::cout << day << std::endl;
    //// Which is obvious:
    ////boost::iterator_range<day_iterator> rng = year;
    ////boost::filtered_range<is_sunday, boost::iterator_range<day_iterator>> flt = 
    ////    rng | boost::adaptors::filtered(is_sunday{});

    //boost::any_range<date, boost::forward_traversal_tag> rng{year};
    //bool const switchOnlyMondays = true;
    //if (switchOnlyMondays)
    //    rng = rng | boost::adaptors::filtered(is_sunday{});
    //// This one crashes, because there is no good story of ownership & lifetime.
    ////    rng = rng | boost::adaptors::filtered([](date d) { return d.day_of_week() == 0; });
    //// Having "auto const& day" here just crashes!!!
    //for (auto const day : rng)
    //    std::cout << day << std::endl;

    return 0;
}
