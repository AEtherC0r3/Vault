TEMPLATE = app
TARGET = emulator

QT = core gui widgets network

CONFIG += c++11

HEADERS += card_exceptions.hpp smartcard.hpp dock.h dialog.h window.h
SOURCES += card_exceptions.cpp smartcard.cpp dock.cpp dialog.cpp window.cpp main.cpp

HEADERS += ../ed25519/fixedint.h ../ed25519/fe.h ../ed25519/ge.h ../ed25519/sc.h ../ed25519/precomp_data.h ../ed25519/sha512.h ../ed25519/ed25519.h
SOURCES += ../ed25519/fe.c ../ed25519/ge.c ../ed25519/sc.c ../ed25519/sha512.c ../ed25519/keypair.c ../ed25519/sign.c

HEADERS += ../base64/base64.h
SOURCES += ../base64/base64.cpp

