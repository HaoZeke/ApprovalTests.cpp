#include "ApprovalTests/reporters/ReporterFactory.h"
#include "ApprovalTests/reporters/ClipboardReporter.h"
#include "ApprovalTests/reporters/LinuxReporters.h"
#include "ApprovalTests/reporters/MacReporters.h"
#include "ApprovalTests/reporters/WindowsReporters.h"
#include "ApprovalTests/Approvals.h"

#include "doctest/doctest.h"

using namespace ApprovalTests;

TEST_CASE("ReporterFactory creates a Reporter given a string")
{
    ReporterFactory factory;
    auto reporter = factory.createReporter("ClipboardReporter");
    CHECK(dynamic_cast<ClipboardReporter*>(reporter.get()));
}

TEST_CASE("ReporterFactory returns null if reporter is unknown")
{
    ReporterFactory factory;
    auto reporter = factory.createReporter("IDoNotExist");
    CHECK(!reporter);
}

TEST_CASE("ReporterFactory creates a Reporter given Linux::MeldReporter")
{
    ReporterFactory factory;
    auto reporter = factory.createReporter("Linux::MeldReporter");
    CHECK(dynamic_cast<Linux::MeldReporter*>(reporter.get()));
}

TEST_CASE("ReporterFactory creates a Reporter given KDiff3")
{
    ReporterFactory factory;
    for (const auto& reporterName : {"KDiff3", "KDiff3Reporter"})
    {
        INFO(std::string("Reporter name = ") + reporterName);
        auto reporter = factory.createReporter(reporterName);
        if (SystemUtils::isWindowsOs())
        {
            CHECK(dynamic_cast<Windows::KDiff3Reporter*>(reporter.get()));
        }
        else if (SystemUtils::isMacOs())
        {
            CHECK(dynamic_cast<Mac::KDiff3Reporter*>(reporter.get()));
        }
        else
        {
            CHECK(dynamic_cast<Linux::KDiff3Reporter*>(reporter.get()));
        }
    }
}

TEST_CASE("ReporterFactory creates a Reporter given Windows::WinMergeReporter")
{
    ReporterFactory factory;
    auto reporter = factory.createReporter("Windows::WinMergeReporter");
    CHECK(dynamic_cast<Windows::WinMergeReporter*>(reporter.get()));
}

TEST_CASE("ReporterFactory creates a Reporter given Windows::WinMerge")
{
    ReporterFactory factory;
    auto reporter = factory.createReporter("Windows::WinMerge");
    CHECK(dynamic_cast<Windows::WinMergeReporter*>(reporter.get()));
}

TEST_CASE("Verify all valid env variable values - on Windows")
{
    ReporterFactory reporterFactory;
    Approvals::verifyAll("",
                         reporterFactory.allSupportedReporterNames(),
                         [](auto input, auto& stream) { stream << input; });
}

TEST_CASE("Find a valid reporter - on Windows")
{
    ReporterFactory reporterFactory;

    std::vector<std::string> inputs{
        "Mac::KaleidoscopeReporter",
        "KaleidoscopeReporter",
        "Mac::Kaleidoscope",
        "Kaleidoscope",
    };

    Approvals::verifyAll("", inputs, [&](auto input, auto& stream) {
        stream << input << " => " << reporterFactory.findReporterName("Windows::", input);
    });
}

TEST_CASE("Find a valid reporter, with leading and trailing space")
{
    ReporterFactory reporterFactory;

    std::vector<std::string> inputs{
        "Kaleidoscope",
        " Kaleidoscope",
        "Kaleidoscope ",
        " Kaleidoscope ",
    };

    Approvals::verifyAll("", inputs, [&](auto input, auto& stream) {
        stream << input << " => " << reporterFactory.findReporterName("Mac::", input);
    });
}
