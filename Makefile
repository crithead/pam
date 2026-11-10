# Build a PAM module
CC = /usr/bin/gcc
CFLAGS = -Wall -Werror -std=c11 -O2

ifeq ($(V),1)
E =
else
E = @
endif

ifeq ($(D),1)
CFLAGS += -g
endif

.PHONY: module
modules: activate-pam-session pam_notice.so

.PHONY: all
all: activate-pam-session activate-pam-session.1.gz pam_notice.so pam_notice.8.gz

pam_notice.so: pam_notice.o
	$(E)$(LD) -x --shared -o $@ $^

pam_notice.o: pam_notice.c
	$(E)$(CC) $(CFLAGS) -fPIC -fno-stack-protector -c $^

pam_notice.8.gz: pam_notice.8
	$(E)gzip -k $^

activate-pam-session: activate-pam-session.c
	$(E)$(CC) $(CFLAGS) -o $@ $^ -lpam -lpam_misc

activate-pam-session.1.gz: activate-pam-session.1
	$(E)gzip -k $^

.PHONY: install
install:
	$(E)install -u root -g root -m 0755 pam_notice.so /usr/lib64/security
	$(E)install -u root -g root -m 0644 pam_notice.8.gz /usr/share/man/man8
	$(E)install -u root -g root -m 0755 aps /usr/bin
	$(E)ln -s activate-pam-session /usr/bin/aps
	$(E)install -u root -g root -m 0644 activate-pam-session.1.gz /usr/share/man/man1
	$(E)ln -s activate-pam-session.1.gz /usr/share/man/man1/aps.1.gz

.PHONY: uninstall
uninstall:
	$(E)rm -f /usr/lib64/security/pam_notice.so
	$(E)rm -f /usr/share/man/man8/pam_notice.8.gz
	$(E)rm -f /usr/share/man/man1/{activate-pam-session,aps}.1.gz
	$(E)rm -f /usr/bin/{activate-pam-session,aps}

.PHONY: clean
clean:
	$(E)rm -f pam_notice.so pam_notice.o pam_notice.8.gz
	$(E)rm -f activate-pam-session activate-pam-session.1.gz

