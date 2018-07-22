TINOS build 206
---------------

Please read it before you play with TINOS.

This is TINOS, build 206. It has some new features:
- almost full FAT filesystem support,
- asynchronous IO, however still untested
- richer C library, but not yet complete

Please fiddle with it some, and send to me any comments, problems
or bug findings you might come across. Any word from you appreciated.

Please note, that this is a snapshot of my work. This is not complete
release, so that you should treat it as a piece of work that you
might wanna look at and experiment with, but certainly not to use
as a complete software product. 

WARNING
-------

This version of TINOS has full MS-DOG filesystem support. Since
that code is untested, please be careful when fiddling with it.
I recommend you first run it on a partition that you won't be crying
for when it's wasted. TINOS handles DOS extended partitions. For info
how to select working partition, see PARTITIONS section.

Apart of wrecking your favorite FAT partition, you can't make any
harm to your PC with this product. (I guess :-))

PARTITIONS
----------

TINOS supports FAT filesystem. It can handle primary and extended partitions.
After TINOS kernel boots, it tries to mount DOS partition (either primary
or extended). TINOS supports only one hard disk. (primary)

In /etc subdir of TINOS distribution you will find a file 'bootp'.
This is a way of telling kernel things before it boots. For now
it takes partition number from this file (specifically, coffload.exe
reads this file and puts it somewhere in memory, so that a kernel finds it
later). By default the partition number is 0, so TINOS will try to mount 
the first partition on the disk. The numbering order is that of partition table
on the first sector of disk (e.g. fdisk or Norton Disk Editor will tell
you that). The numbering starts from 0, so possible numbers are: 0, 1, 2 or 3.

SO, prior to running it, verify that etc/bootp contains the right number.

TINOS
-----

After you booted it successfully, you can play with it. Well, I know that
the sample apps I have provided suck heavily, but I can hardly find some
time for it. Writing kernel itself is a bitch!

You can help me a lot if you write or port some app to TINOS. The C library
isn't full, but nontheless it has something.

The IO functions are not fully implemented yet, e.g. they do not check
for permissions when read() or write(). Also no locks are held, so when
you read and write simultaneously - it's gonna be a mess in a file I guess.
