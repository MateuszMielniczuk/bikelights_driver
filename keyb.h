//***********************************************************************
// Plik: keyb.h
//
// Zaawansowana obsługa przycisków i klawiatur
// Wersja:    1.0
// Licencja:  GPL v2
// Autor:     Deucalion
// Email:     deucalion#wp.pl
// Szczegóły: http://mikrokontrolery.blogspot.com/2011/04/jezyk-c-biblioteka-obsluga-klawiatury.html
//
//***********************************************************************


#ifndef _KEYB_H_

#define _KEYB_H_


#define KEY0		(1<<PD0)
#define KEY1		(1<<PD1)
#define KEY2		(1<<PD2)
#define KEY3		(1<<PD3)
#define KEY4		(1<<PD4)
#define KEY5		(1<<PD5)
#define KEY6		(1<<PD6)

#define	KEY_BACK		KEY0
#define	KEY_SHORT		KEY1
#define	KEY_LONG		KEY2
#define	KEY_HORN		KEY3
#define	KEY_BREAK		KEY4
#define KEY_LEFT		KEY5
#define KEY_RIGHT		KEY6

#define ANYKEY		(KEY0 | KEY1 | KEY2 | KEY3 | KEY4 | KEY5 | KEY6)
#define KEY_MASK	(KEY0 | KEY1 | KEY2 | KEY3 | KEY4 | KEY5 | KEY6)

#define KBD_LOCK	1
#define KBD_NOLOCK	0

#define KBD_DEFAULT_ART	((void *)0)

void
ClrKeyb( int lock );

unsigned int
GetKeys( void );


unsigned int
KeysTime( void );


unsigned int
IsKeyPressed( unsigned int mask );


unsigned int

IsKey( unsigned int mask );


void
KeybLock( void );

void
KeybSetAutoRepeatTimes( unsigned short * AutoRepeatTab );


#endif
