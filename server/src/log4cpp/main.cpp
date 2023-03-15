#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/PatternLayout.hh>
#include <iostream>

int main()
{ 
    // OstreamAppender:输出到输出流中（输出到控制台上）
    // FileAppender：输出到文件中
    // RollingFileAppender：输出到文件中并且能够设置文件最大超过多少时生成新文件
    // DailyRollingFileAppender：每天生成一个新文件
    // NTEventLogAppender：将日志输出到Windows事件日志中去
    // ...
    log4cpp::Category& root = log4cpp::Category::getRoot();
    // 子分类subCat1
    log4cpp::Category& sub1 = log4cpp::Category::getInstance("sub1");
    root.setPriority(log4cpp::Priority::DEBUG);
    log4cpp::PatternLayout *patternLayout = new log4cpp::PatternLayout();
    patternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} %t [%p] %m %n");

    log4cpp::Appender *appender = new log4cpp::FileAppender("test", "./log4cppTest.log");
    root.addAppender(appender);
    appender->setLayout(patternLayout);

    root.error("test root error");
    root.warn("test root error");
    root.info("test root error");

    sub1.fatal("test sub1 error");
    sub1.notice("test sub1 error");
    sub1.debug("test sub1 error");

}