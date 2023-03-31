
dd if=/dev/zero of=/tmp/testFile bs=16K count=1

(sleep 3; ./target)&
./runner
