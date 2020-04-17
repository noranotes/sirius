AS=nasm


.PHONY: clearscreen stage0.bin create disk mount push test

#compiler boot
stage0.bin: stage0/stage0.asm
	$(AS) -f bin -o $@ $<
move:
	mv *.bin bin
clean:
	rm bin/*.bin
	rm bin/*.sys

create:
	qemu-img create -f raw part.img 48M
disk:
	qemu-img create -f raw disk.img 512M

mount:
	#sudo ./kmbr bin/stage0.bin /dev/sdb
	mkdosfs part.img
	sudo mount part.img /mnt
	sudo cp bin/stage0.bin /mnt/
	sudo cp bin/stage1.bin /mnt/
	sudo cp bin/kernel.bin /mnt/
	sudo cp bmp/*.bmp /mnt/
	#sudo cp bin/task.sys /mnt/
	#sudo cp bin/gserver.sys /mnt/
	#sudo cp bin/files.sys /mnt/
	#sudo cp bin/msgbox.sys /mnt/
	#sudo cp bin/terminal.sys /mnt/
	#sudo cp bin/shell.sys /mnt/
	sudo cp bin/*.sys /mnt/
	sudo cp bin/test.bin /mnt/
	sudo cp bin/fasm.bin /mnt/
	sudo cp bin/nasm.bin /mnt/
	sudo cp bmp/ubuntu.ttf /mnt/
	sudo cp bmp/w.jpg /mnt/
	sudo cp bmp/a.ppm /mnt/
	sudo cp bmp/b.ppm /mnt/
	sudo cp bin/nanojpeg.bin /mnt/
	sudo cp bin/c4.bin /mnt/
	sudo cp main.asm /mnt/
	sudo umount /mnt

	./pcopy bin/stage0.bin part.img sirius.vhd
	rm part.img
test:
	#sudo qemu-system-i386 -m 64 -drive file=/dev/sdb,format=raw
	sudo qemu-system-i386 -m 64 -drive file=sirius.vhd,format=raw -hdb disk.img

clearscreen:
	clear



push:
	git add ./
	git commit -m "commit++"
	git push -u origin master
