//
// Created by no3core on 2024/4/27.
//

#include "../src/GBNSender.h"
#include "GBNSenderTestHarness.h"
#include <cassert>
#include <iostream>



int main(){
    // test for the datapkg size is 16 bytes


    setbuf(stdout,0);
    {
        // no time test
        GBNSender test(5,100);
        GBNSenderTestHarness testHarness(test);
        // add basic data into it
        test.GetStream().Write(
                "\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11"
                "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"
                "\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33"
                "\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44"
                "\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55"
                "\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66"
                "\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77"
        );
        test.FillWindow();

        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);
        test.AckReceived(0);
        testHarness.ExpectPDU().Num(5);
        testHarness.ExpectNoPDU();

        test.AckReceived(6);
        testHarness.ExpectNoPDU();

        test.AckReceived(5);
        testHarness.ExpectPDU().Num(6);
        testHarness.ExpectNoPDU();

        test.FillWindow();
        test.AckReceived(10);
        testHarness.ExpectNoPDU();
        test.GetStream().Write("0123456789ABCDEF""FEDCBA9876543210");
        test.GetStream().Write("FEDCBA98");
        test.FillWindow();

        testHarness.ExpectPDU().Num(7).Data("0123456789ABCDEF");
        testHarness.ExpectPDU().Num(8).Data("FEDCBA9876543210");
        testHarness.ExpectPDU().Num(9).Data("FEDCBA98");

        test.AckReceived(4);
        test.AckReceived(4);
        test.AckReceived(7);
        testHarness.ExpectNoPDU();

        test.FillWindow();
        testHarness.ExpectNoPDU();
    }


    {
        // add the basic timer test:
        GBNSender sender(5,100);
        GBNSenderTestHarness testHarness(sender);
        sender.GetStream().Write(
                "\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11"
                "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"
                "\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33"
                "\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44"
                "\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55"
                "\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66"
                "\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77"
                "\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88"
                "\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99"
                "\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa"
        );
        sender.FillWindow();

        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);

        sender.TimeElasped(99);
        sender.AckReceived(2);

        testHarness.ExpectPDU().Num(5);
        testHarness.ExpectPDU().Num(6);
        testHarness.ExpectPDU().Num(7);

        // timeout, need retrans seq:3--7
        sender.TimeElasped(101);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);
        testHarness.ExpectPDU().Num(5);
        testHarness.ExpectPDU().Num(6);
        testHarness.ExpectPDU().Num(7);

        sender.TimeElasped(101);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);
        testHarness.ExpectPDU().Num(5);
        testHarness.ExpectPDU().Num(6);
        testHarness.ExpectPDU().Num(7);

        sender.TimeElasped(99);
        // refresh the timer here
        sender.AckReceived(4);

        testHarness.ExpectPDU().Num(8);
        testHarness.ExpectPDU().Num(9);

        // no timeout here
        sender.TimeElasped(1);

        testHarness.ExpectNoPDU();

        sender.TimeElasped(99);
        testHarness.ExpectPDU().Num(5);
        testHarness.ExpectPDU().Num(6);
        testHarness.ExpectPDU().Num(7);
        testHarness.ExpectPDU().Num(8);
        testHarness.ExpectPDU().Num(9);

        sender.AckReceived(9);


        sender.GetStream().Write(
                "\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11"
                "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"
                "\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33"
                "\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44""\x44"
                "\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55""\x55"
                "\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66""\x66"
                "\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77""\x77"
                "\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88""\x88"
                "\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99""\x99"
                "\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa""\xaa"
        );
        sender.FillWindow();

        sender.TimeElasped(99);
        testHarness.ExpectPDU().Num(10);
        testHarness.ExpectPDU().Num(11);
        testHarness.ExpectPDU().Num(12);
        testHarness.ExpectPDU().Num(13);
        testHarness.ExpectPDU().Num(14);

        sender.AckReceived(14);
        sender.TimeElasped(1);
        testHarness.ExpectPDU().Num(15);
        testHarness.ExpectPDU().Num(16);
        testHarness.ExpectPDU().Num(17);
        testHarness.ExpectPDU().Num(18);
        testHarness.ExpectPDU().Num(19);
    }

    {
        // time test for starter package
        GBNSender sender(5,100);
        GBNSenderTestHarness testHarness(sender);
        sender.GetStream().Write(
                "\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11"
                "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"
                "\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33"
        );
        sender.FillWindow();

        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);

        sender.TimeElasped(101);

        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);

        sender.GetStream().Write(
                "\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11"
                "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"
                "\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33"
        );
        sender.FillWindow();
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);

        sender.TimeElasped(101);
        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);

        sender.FillWindow();
        sender.TimeElasped(101);
        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);

        sender.GetStream().Write(
                "\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11""\x11"
                "\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22""\x22"
                "\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33""\x33"
        );
        sender.FillWindow();

        sender.TimeElasped(101);
        testHarness.ExpectPDU().Num(0);
        testHarness.ExpectPDU().Num(1);
        testHarness.ExpectPDU().Num(2);
        testHarness.ExpectPDU().Num(3);
        testHarness.ExpectPDU().Num(4);

        sender.AckReceived(4);

        sender.TimeElasped(101);
        testHarness.ExpectPDU().Num(5);
        testHarness.ExpectPDU().Num(6);
        testHarness.ExpectPDU().Num(7);
        testHarness.ExpectPDU().Num(8);

    }
}