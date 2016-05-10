#include "Interpolate.hpp"
#include <iostream>
#include <typeinfo>
#include <locale>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <cstring>
// Needed by {set,get}rlimit().
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

constexpr unsigned MEMORY_LIMIT = 1024*1024*30;

class A {
    friend int main(int argc, char **argv);
    friend std::ostream & operator<<(std::ostream &os, const A &a) {
        return os << a.i;
    }
    A(int i_) : i(i_) {}
    A(const A &) = delete;
    A &operator=(const A &) = delete;
    const int i;
};

class B {
    friend int main(int argc, char **argv);
    friend std::ostream & operator<<(std::ostream &os, const B &a) {
        return os << a.str;
    }
    B(std::string s) : str(std::move(s)) {}
    B(const B &) = delete;
    B &operator=(const B &) = delete;
    const std::string str;
};

// This class is used to test space efficiency.
class Out {
   public:
      Out(std::size_t mem_amount) : mem_amount(mem_amount) {}
      const std::size_t mem_amount;
};
std::ostream &
operator<<(std::ostream &os, const Out &o) {
    const std::string buf(1024*1024, 'A');
    for (std::size_t i = 0; i < o.mem_amount; i++) {
        os << buf;
    }
    return os;
}

template <typename... Ts>
void test(const char *func, int line_no, const std::string &cmp, const std::string &fmt, Ts &&...params) {
    std::stringstream s;
    s << cs540::Interpolate(fmt.c_str(), std::forward<Ts>(params)...);
    if (s.str() != cmp) {
        std::cerr << "Comparison failed at " << func << ":" << line_no << ":\n";
        std::cerr << "    Correct result: \"" << cmp << "\"\n";
        std::cerr << "    Actual result: \"" << s.str() << "\"\n";
    }
}
#define CS540_TEST(...) test(__FUNCTION__, __LINE__, __VA_ARGS__)

int
main(int argc, char **argv) {

    int rv;
    bool do_mem_test = false;

    {
        int c;
        while((c = getopt(argc, argv, "m")) != -1) {
            switch(c) {
                case 'm' :
                do_mem_test = true;
                break;
            case '?' :
                std::cout << "Unrecognized option " << isprint(optopt) << ". The valid options are:\n"
                 "\t-m\t:\tEnables large memory test.\n";
                return 1;
                break;
            default :
                abort();
                break;
            }
        }
    }

    if (do_mem_test) {

        // Limit the amount of memory that can be used, so as not to kill the
        // machine when running the space efficiency test.
        struct rlimit limit;
        rv = getrlimit(RLIMIT_AS, &limit); assert(rv == 0);
        limit.rlim_cur = MEMORY_LIMIT;
        rv = setrlimit(RLIMIT_AS, &limit); assert(rv == 0);
    }

    using namespace cs540;

    CS540_TEST("", "");

    CS540_TEST(R"(\)", R"(\)");
    CS540_TEST(R"(%)", R"(\%)");
    CS540_TEST(R"(\\)", R"(\\)");
    CS540_TEST(R"(foo)", R"(foo)");
    CS540_TEST("\n", "\n");
    CS540_TEST(R"(\%)", R"(\\%)");
    // Test if it returns ostream.
    {
        std::stringstream s;
        s << Interpolate("i=%", 1234) << ", foo" << std::endl;
        assert(s.str() == "i=1234, foo\n");
    }
    CS540_TEST("1234", "%", 1234);
    CS540_TEST(" 1234", " %", 1234);
    CS540_TEST("1234 ", "% ", 1234);
    CS540_TEST(" 1234 ", " % ", 1234);

    CS540_TEST("i=1, x=3.49887, s=foo, a=768, b=hello, c=x", "i=%, x=%, s=%, a=%, b=%, c=%", 1, 3.4988678671, "foo", A(768), B("hello"), 'x');

    // Test rvalue/lvalue.
    {
        int i1 = 12376;
        const int i2 = 778;
        CS540_TEST("i=8798, j=12376, k=778", "i=%, j=%, k=%", 8798, i1, i2);
    }

    // Many args.
    CS540_TEST(
        "56789 3.14 1234 Z hello 313 goodbye -31 1.99 0xffff7777 "
        "56789 3.14 1234 Z hello 313 goodbye -31 1.99 0xffff7777 "
        "56789 3.14 1234 Z hello 313 goodbye -31 1.99 0xffff7777 "
        "56789 3.14 1234 Z hello 313 goodbye -31 1.99 0xffff7777 "
        "56789 3.14 1234 Z hello 313 goodbye -31 1.99 0xffff7777",
        "% % % % % % % % % % "
        "% % % % % % % % % % "
        "% % % % % % % % % % "
        "% % % % % % % % % % "
        "% % % % % % % % % %",
        56789, 3.14, short(1234), 'Z', "hello", A(313), B("goodbye"), -31, 1.99F, (void *)0xffff7777,
        56789, 3.14, short(1234), 'Z', "hello", A(313), B("goodbye"), -31, 1.99F, (void *)0xffff7777,
        56789, 3.14, short(1234), 'Z', "hello", A(313), B("goodbye"), -31, 1.99F, (void *)0xffff7777,
        56789, 3.14, short(1234), 'Z', "hello", A(313), B("goodbye"), -31, 1.99F, (void *)0xffff7777,
        56789, 3.14, short(1234), 'Z', "hello", A(313), B("goodbye"), -31, 1.99F, (void *)0xffff7777);

    // Test too many args.
    try {
        std::stringstream s;
        s << Interpolate("i=%, j=%", 1, 2, 3);
        assert(false);
    } catch (cs540::WrongNumberOfArgs) {
        // std::cout << "Caught exception due to too many args." << std::endl;
    }

    // Test too few.
    try {
        std::stringstream s;
        s << Interpolate("i=%, j=%, k=%", 1, 2);
        assert(false);
    } catch (cs540::WrongNumberOfArgs) {
        // std::cout << "Caught exception due to few args." << std::endl;
    }

    /*
     * Manipulators.
     */

    CS540_TEST("1, 0, false, true, 0, 1", "%, %, %, %, %, %", true, false, std::boolalpha, false, true, std::noboolalpha, false, true);
    CS540_TEST("0x2134, f78", "%, %", std::showbase, std::hex, 0x2134, std::noshowbase, 0xf78);
    CS540_TEST("1, 1.00000, 1", "%, %, %", 1.0, std::showpoint, 1.0, std::noshowpoint, 1.0);
    CS540_TEST("1, 2, +1, +3", "%, %, %, %", 1.0, 2, std::showpos, 1.0, 3, std::noshowpos);
    CS540_TEST("AA, aa", "%, %", std::hex, std::uppercase, 0xaa, std::nouppercase, 0xaa);
    CS540_TEST("3.14e-15, 0xfffff115, 3.14E-15, 0XFFFFF115", "%, %, %, %", std::showbase, std::hex, 3.14e-15, -3819, std::uppercase, 3.14e-15, -3819);

    // std::unitbuf, std::nounitbuf, std::internal, std::left, std::right,
    // std::dec, std::hex, std::oct, std::fixed, std::scientific,
    // std::hexfloat, and std::defaultfloat tested, but shouldn't really be
    // missing anything, since they have the same type as a number of other
    // manipulators: std::ios_base &(std::ios_base &).

    // Test std::ends. This consumes a % sign.
    {
        char str[] = {'a', 'b', 'x', '\0', 'y', 'c', 'd'};
        std::string cmp(str, sizeof str);
        CS540_TEST(cmp, "ab%%%cd", 'x', ffr(std::ends), 'y');
    }

    // Test std::flush. This does not consume a % sign.
    {
        // It'd be better to check that the flush actually occurred, but that
        // would be harder.
        std::cout << Interpolate("%", "std::flush\n", ffr(std::flush));
    }

    // Test std::endl. This also consumes a % sign.
    CS540_TEST("i=1%, x=3.4989, s=foo\n true 1 1234\\",
     R"(i=%\%, x=%, s=%% % % %\)",
     1, std::setprecision(5), 3.4988678671, "foo", ffr(std::endl), std::boolalpha, true, std::noboolalpha, true, A(1234));

    // Tests both std::setiosflags() and std::resetiosflags()
    CS540_TEST("abc, 1234", "%, %",
     std::resetiosflags(std::ios_base::basefield),
     std::setiosflags(std::ios_base::hex),
     0xabc,
     std::resetiosflags(std::ios_base::basefield),
     1234);

    // Test std::setbase().
    CS540_TEST("aa, 123, 999", "%, %, %",
     std::setbase(16), 0xaa,
     std::setbase(8), 0123,
     std::setbase(10), 999);

    // Test std::setfill(), std::setprecision(), and std::setw().
    CS540_TEST("--1.234567899", "%", std::setw(13), std::setprecision(10), std::setfill('-'), 1.234567899);

    // Test std::put_money().
    {
        std::stringstream ss;
        ss.imbue(std::locale("en_US.utf8"));
        ss << Interpolate("%, %", std::showbase, std::put_money(112), std::put_money(112, true));
        // std::cout << ss.str() << std::endl;
        assert(ss.str() == "$1.12, USD  1.12");
    }
    // Test std::put_time().
    {
        std::stringstream ss;
        std::tm tm;
        std::memset(&tm, 0, sizeof(tm));
        tm.tm_sec = 44;
        tm.tm_min = 53;
        tm.tm_hour = 13;
        tm.tm_mday = 2;
        tm.tm_mon = 4;
        tm.tm_year = 116;
        tm.tm_wday = 1;
        tm.tm_yday = 122;
        tm.tm_isdst = 1;
        std::tm tm2{tm};
        tm2.tm_sec = 1; // Make time a bit different.
        ss.imbue(std::locale("en_US.utf8"));
        ss << Interpolate("%, %", std::put_time(&tm, "%c %Z"), std::put_time(&tm2, "%c %z"));
        // std::cout << ss.str() << std::endl;
        ss << " ";
        ss.imbue(std::locale("C"));
        ss << Interpolate("%, %", std::put_time(&tm, "%c %Z"), std::put_time(&tm2, "%c %z"));
        // std::cout << ss.str() << std::endl;
        assert(ss.str() == "Mon 02 May 2016 01:53:44 PM EDT EDT, Mon 02 May 2016 01:53:01 PM EDT +0000 Mon May  2 13:53:44 2016 EDT, Mon May  2 13:53:01 2016 +0000");
    }

    // Tests std::quoted().
    CS540_TEST(R"("\"A backslash: \\\"")", "%", std::quoted("\"A backslash: \\\""));

    // Verify that the operations are applied to the actual ostream.
    {
        std::stringstream ss;
        ss << Interpolate("", std::showbase);
        ss << Interpolate("% ", 11, std::hex);
        ss << 0xabc;
        ss << Interpolate(" %", 0xaa);
        ss << Interpolate(" ", std::dec);
        ss << 123;
        ss << Interpolate("", std::oct);
        ss << " " << 0567;
        // std::cout << ss.str() << std::endl;
        assert(ss.str() == "11 0xabc 0xaa 123 0567");
    }

    // Test space efficiency.
    {
        std::fstream out("/dev/null");
        if (do_mem_test)
           out << Interpolate("%", Out{4*512*1024});
        else
           out << Interpolate("%", Out{1024});
    }
}
