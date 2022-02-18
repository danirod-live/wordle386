# WordleOS (ES)

![Screenshot](screenshot.png)

This is Wordle implemented as a Multiboot application.
You load the executable file using GNU GRUB and you get a fullscreen Wordle
that run without an operating system.

So it is like... its own operating system I guess?

**Gringo alert**: it is using a Spanish dictionary. You have been warned.

## Why the code looks so weird?

I copied the directory structure from my other operating system repository,
[NativeOS](https://github.com/danirod/nativeos), so that I could re-use the
functions for drawing to the screen or reading from the keyboard. I just removed the main function and replaced it with a call to wordle() to start the game.

That is why at the moment it looks like this. The thing you are looking for is in the `kernel/kern/wordle.c` and
the list of words is at `kernel/kern/words.h`.

I only modified the files that I need to touch, so everything else
looks the same. I did not even rename the executable files or the
comments LUL.


I did this in a [livestream](https://twitch.tv/danirod_) in a couple hours.
Please be considerate at me. I might cleanup this in the future. Or not.

To download a ISO, check the releases for an ISO that you can boot
in a virtual machine like QEMU, or in an old computer. I suspect this is
not going to work in EFI systems because I don't know EFI yet.

To build this, you are going to need clang. Run `make build-kernel PROFILE=I386`
and you will have the multiboot executable in `dist/I386/boot/nativeos.exe`
(don't ask about the file name and the extension, I told you, everything is
copied and pasted!). You will be able to boot this using GRUB, for instance,
`kernel /tmp/nativeos.exe` in a GRUB shell.
