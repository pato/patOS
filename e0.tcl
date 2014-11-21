#!/usr/bin/expect

set timeout 10

expect_after {
    timeout {
        puts "----> timeout <----\r"
        exit
    }
}

spawn qemu-system-x86_64 -nographic --serial mon:stdio -hdc kernel/kernel.img -hdd fat439/user.img

expect "shell> "
send "ls\r"

expect "shell> "
send "echo This is cool\r"

expect "shell> "
send "    echo    I know    how  to         handle  spaces   \r"

expect "shell> "
send "cat f1.txt\r"

expect "shell> "
send " f1.txt\r"

expect "shell> "
send "cat f1.txt f2.txt\r"

expect "shell> "
send "f2.txt f1.txt\r"

expect "shell> "
send "cat f1.txt f2.txt f1.txt f2.txt\r"

expect "shell> "
send "gcc\r"

expect "shell> "
send "cat notThere\r"

expect "shell> "
send "shutdown\r"

expect "*** System Shutdown ***\r"
send \001
send "x"
