CC=g++

build: usb-guard.cpp
	@$(CC) usb-guard.cpp -o usb-guard

install: usb-guard
	@echo "Creating Directory."
	@mkdir -p /usr/share/USB-Guard
	@echo "Copying Files."
	@cp usb-guard /usr/bin/usb-guard
	@echo "#Write your bash script here." > /usr/share/USB-Guard/script.sh
	@echo "ACTION==\"add\", ATTRS{idVendor}==\"****\", ATTRS{idProduct}==\"****\", RUN+=\"/usr/bin/usb-guard -x\"" > /etc/udev/rules.d/usb-guard.rules
	@echo "Setting Permitions."
	@chmod 755 /usr/bin/usb-guard
	@chmod 755 -R /usr/share/USB-Guard
	@echo "Creating Whitelist."
	@usb-guard -a
	@echo "Enabling USB-Guard."
	@usb-guard -e

.PHONY: clean

uninstall: /usr/bin/usb-guard
	@echo "Uninstalling USB-Guard."
	@rm -rf /usr/share/USB-Guard
	@rm -f /etc/udev/rules.d/usb-guard.rules
	@rm -f /usr/bin/usb-guard
	@echo "We are sorry to see you leave."
	@echo "Please send your feedback to <pranavbhaskar23@gmail.com>."

clean: 
	@rm -f ./usb-guard
