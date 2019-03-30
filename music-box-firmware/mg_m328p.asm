;----------------------------------------------------------;
; Melody Generator  (C)ChaN, 2005

.include "m328Pdef.inc"	;This is included in "Atmel AVR Studio"
.include "avr.inc"
.include "mg.inc"


.def	_0	= r15
.def	_Sreg	= r14
.def	_Zreg	= r12
.def	_Yreg	= r10
.def	_TmrE	= r6
.def	_TmrH	= r9
.def	_TmrL	= r8
.def	_TmrS	= r7

.equ	N_NOTE	= 8

;#define VAR_EV_DELTA
.equ	envelopeDelta	= 5
#define USE_HARDWRARE_MUL
#define USE_ANTOHER_PWM_MOD_METHOD


;----------------------------------------------------------;
; Work Area

.dseg
	.org	RAMTOP100
NoteIdx:.byte	1	; Note rotation index

Notes:	.byte	(2+3+1+1+1+1+1)*N_NOTE
.equ	ns_freq = 0	;Angular Speed
.equ	ns_rptr = 2	;Wave table read pointer (16.8 fraction)
.equ	ns_lvl = 5	;Level
.equ	ns_wrap = 6	;Loop Flag
.equ	ns_loop = 7	;Loop Count
.equ	ns_lp = 8	;Level Pointer
.equ	ns_ev_dly_offest = 9	;Level Pointer
.equ	nsize = 10	;size of this structure

TickCounter: .byte 3 ;


;----------------------------------------------------------;
; Program Code

.cseg
	; Interrupt Vectors (Atmega328p)
	jmp reset ; Reset Handler
	jmp 0	;External Interrupt Request 0
	jmp 0	;External Interrupt Request 1
	jmp 0	;Pin Change Interrupt Request 0
	jmp 0	;Pin Change Interrupt Request 1
	jmp 0	;Pin Change Interrupt Request 2
	jmp 0	;Watchdog Time-out Interrupt
	jmp 0	;Timer/Counter2 Compare Match A
	jmp 0	;Timer/Counter2 Compare Match B
	jmp 0	;Timer/Counter2 Overflow
	jmp 0	;Timer/Counter1 Capture Event
	jmp 0	;Timer/Counter1 Compare Match A
	jmp 0	;Timer/Counter1 Compare Match B
	jmp 0	;Timer/Counter1 Overflow
	jmp isr_tc0_coma	;Timer/Counter0 Compare Match A
	jmp 0	;Timer/Counter0 Compare Match B
	jmp 0	;Timer/Counter0 Overflow
	jmp 0	;SPI Serial Transfer Complete
	jmp 0	;USART Rx Complete
	jmp 0	;USART Data Register Empty
	jmp 0	;USART Tx Complete
	jmp 0	;ADC Conversion Complete
	jmp 0	;EEPROM Ready
	jmp 0	;Analog Comparator
	jmp 0	;Two-wire Serial Interface
	jmp 0	;Store Program Memory Read

;--------------------------------------------------------------------;
; Program Code

reset:
	clr	_0
	ldiw	X, RAMTOP100		;Clear RAM
	ldi	AL, 0			;
	st	X+, _0			;
	dec	AL			;
	brne	PC-2			;/


	ldi	r16, low(RAMEND)
	out	SPL, r16
	ldi	r16, high(RAMEND)
	out	SPH, r16	

;	outi	OSCCAL, 172		;Adjust OSCCAL if needed.

	outi	PORTB, 0b001101		;Initalize Port B
	outi	DDRB,  0b000111		;/

	ldi AL,0b10100001
	sts TCCR1A,AL
	;ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10 ; Fast PWM pclk/1
	ldi AL,0b00001001
	sts TCCR1B,AL

	outi	OCR0A, 62		;Initalize TC0 in 32 kHz interval timer ( pclk=16M )
	outi	TCCR0A, 0b00000010
	outi	TCCR0B, 0b00000010 ;pclk/8
	ldi AL,(1<<OCIE0A)
	sts	TIMSK0, AL


start_play:
	ldiw	Z, score*2
	cli
	clrw	_Tmr
	clr	_TmrE
	clr	_TmrS

	ldiw	X,TickCounter
	st X+,_0
	st X+,_0
	st X,_0
	sei
pl_next:
	cli
	mov T4L,_0
	mov T4H,_0
accumlate_var_len_tick:
	lpm AL,Z+
	add T4L,AL
	adc T4H,_0
	cpi AL,255
	breq accumlate_var_len_tick
	ldiw X,TickCounter
	ld BL,X+
	ld BH,X+
	ld DL,X

	add BL,T4L
	adc BH,T4H
	adc DL,_0
	ldiw X,TickCounter
	st X+,BL
	st X+,BH
	st X,DL
	sei
	rcall	drv_decay
	cli
	cpw	_Tmr, B
	cpc	_TmrE,DL
	sei
	brcs	PC-6

pl_note:
	lpm	CL, Z+
	cpi	CL, EoS
	breq	stop_play
	mov	AL, CL
	rcall	note_on
	andi	CL, en
	breq	pl_note
	rjmp	pl_next

stop_play:
	outi SMCR,0b00000101
	outi	DDRB,  0b000000		;/
	;sei
	sleep
	;rjmp stop_play



;--------------------------------------------------------------------;
; Note ON
;
;Call: AL[6:0] = key number

note_on:
	pushw	Z
#ifdef VAR_EV_DELTA
	mov DH, AL
#endif
	mov	ZL, AL
	lsl	ZL
	clr	ZH
	addiw	Z, tbl_pitch*2
	lpmw	A, Z+
#ifdef VAR_EV_DELTA
	mov	ZL, DH
	clr	ZH
	addiw	Z, tbl_ev_dly_offest*2
	lpm DH,	Z
#endif
	lds	YL, NoteIdx
	addi	YL, nsize
	cpi	YL, nsize*N_NOTE
	brcs	PC+2
	clr	YL
	sts	NoteIdx, YL
	clr	YH
	addiw	Y, Notes
	ldiw	B, wt_attack*2
	cli
#ifdef VAR_EV_DELTA
	std		Y+ns_ev_dly_offest,DH
#endif
	stdw	Y+ns_freq, A
	stdw	Y+ns_rptr+1, B
	sei
	stdi	Y+ns_lvl, 255
	std	Y+ns_wrap, AL
	std	Y+ns_loop, _0
	std	Y+ns_lp, _0

	popw	Z
	ret


;--------------------------------------------------------------------;
; Decay envelope generation

drv_decay:
	pushw	Z
	ldiw	Y, Notes
dd_lp:
	ldd	AL, Y+ns_wrap	;Has sustain loop not wrapped?
	ldi	AH, 255		;
	cp	AL, AH		;
	breq	dd_nxt		;/
	std	Y+ns_wrap, AH	;Clear wrapped flag.
	ldd	AL, Y+ns_loop
#ifdef VAR_EV_DELTA
	ldd DH,Y+ns_ev_dly_offest
	inc	AL
	cp	AL, DH
#else
	inc	AL
	cpi	AL, envelopeDelta
#endif
	brcs	PC+2
	ldi	AL, 0
	std	Y+ns_loop, AL
	brcs	dd_nxt
	ldd	ZL, Y+ns_lp
	inc	ZL
	breq	dd_nxt
	std	Y+ns_lp, ZL
	clr	ZH
	addiw	Z, envelope*2
	lpm	AL, Z
	std	Y+ns_lvl, AL
dd_nxt:	adiw	YL, nsize
	cpi	YL, low(Notes+nsize*N_NOTE)
	brne	dd_lp

	popw	Z
	ret



;--------------------------------------------------------------------;
; 32 kHz wave form synthesising interrupt


isr_tc0_coma:
	in	_Sreg, SREG		;Save regs...
	movw	_Zreg, ZL		;
	movw	_Yreg, YL		;/
#ifdef USE_HARDWRARE_MUL	
	pushw A
#endif
	ldiw	Y, Notes		;Process all notes
	clrw	T2			;Clear accumlator
tone_lp:
	ldd	EH, Y+ns_rptr		;Load wave table pointer
	lddw	Z, Y+ns_rptr+1		;/
	lpm	EL, Z			;Get a sample
	lddw	T4, Y+ns_freq		;Load angular speed
	add	EH, T4L			;Increase wave table ptr (next angle)
	adc	ZL, T4H			;
	adc	ZH, _0			;/
	cpi	ZH, high(wt_end*2)	;Repeat sustain area
	brcs	PC+4			;
	subiw	Z, (wt_end-wt_loop)*2	;
	std	Y+ns_wrap, _0		;/
	std	Y+ns_rptr, EH		;Save wave table ptr
	stdw	Y+ns_rptr+1, Z		;/
	ldd	EH, Y+ns_lvl		;Apply envelope curve
#ifdef USE_HARDWRARE_MUL
						;Copy register E to A because MULSU can only use r16 - r23. Register A is AL:r16,AH:r17
	movw AH:AL,EH:EL 
	MULSU AL,AH				;/ Using hardware multiplier
	add T2L,T0H
	ldi EL,0
	sbrc T0H,7
	ldi EL,0xFF
	adc T2H,EL

#else
	MULT	
	addw	T2, T0			;Add the sample to accumlator	
#endif
	adiw	YL, nsize			;Next note
	cpi	YL, low(Notes+nsize*N_NOTE);
	brne	tone_lp			;/

	;asrw	T2			;Divide it by 4
	;asrw	T2			;/
	ldiw	E, 255			;Clip it between -255 to 253
	cpw	T2, E			;
	brlt	PC+2			;
	movw	T2L, EL			;
	ldiw	E, -255			;
	cpw	T2, E			;
	brge	PC+2			;
	movw	T2L, EL			;/
#ifdef USE_ANTOHER_PWM_MOD_METHOD
	sbrc T2H,7
	jmp NEG_NUM
	sts OCR1AL,_0
	sts OCR1BL,T2L
	jmp NEG_NUM_END
NEG_NUM:
	com T2L
	com T2H
	sec
	adc T2L,_0
	adc T2H,_0
	sts OCR1AL,T2L
	sts OCR1BL,_0
NEG_NUM_END:
#else
	asrw	T2			; Set it to PWM modulator ： 把16位的T2带符号位右移一位，除以2，最低位移到Carrier 
	ror	T2H				; 把T2H的高位右移，Carrier进到T2H的第七位
	mov	EL, T2L			; 复制T2L到EL
	subi	EL, 0x80	; EL=EL-0x80
	mov	EH, EL			; 
	com	EH				; EH取反
	sbrc	T2H, 7		; 如果T2H的第七位是0就跳过下面一行
	inc	EL				; EL++
	sts OCR1AL,EL
	sts OCR1BL,EH
#endif

	sec				;Increment sequense timer
	adc	_TmrS, _0		;
	adc	_TmrL, _0		;
	adc	_TmrH, _0		;
	adc _TmrE, _0

	movw	ZL, _Zreg		;Restore regs...
	movw	YL, _Yreg		;
	out	SREG, _Sreg		;/
#ifdef USE_HARDWRARE_MUL
	popw A
#endif
	reti

tbl_ev_dly_offest: ;  A     B     H     C    Cis    D    Dis    E     F    Fis    G    Gis 
	.db  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ; 220Hz..
	.db  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, 1 ; 440Hz..
	.db  2,  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ; 880Hz..
	.db 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 ; 1760Hz..
	.db 3, 3, 3, 3, 3, 3, 3,3			   ; 3520Hz

;--------------------------------------------------------------------;
; Pitch number to angular speed conversion table
;--------------------------------------------------------------------;
;Since sustain area of wave table, a cycle of fundamental frequency, is sampled
;in 128 points, the base frequency becomes 32000/128 = 250 Hz. The wave table
;lookup pointer, 16.8 fraction, is increased every sample by these 8.8 fractional
;angular speed values.

tbl_pitch: ;  A     B     H     C    Cis    D    Dis    E     F    Fis    G    Gis 
	.dw  225,  239,  253,  268,  284,  301,  319,  338,  358,  379,  401,  425 ; 220Hz..
	.dw  451,  477,  506,  536,  568,  601,  637,  675,  715,  758,  803,  851 ; 440Hz..
	.dw  901,  955, 1011, 1072, 1135, 1203, 1274, 1350, 1430, 1515, 1606, 1701 ; 880Hz..
	.dw 1802, 1909, 2023, 2143, 2271, 2406, 2549, 2700, 2861, 3031, 3211, 3402 ; 1760Hz..
	.dw 3604, 3818, 4046, 4286, 4542, 4812, 5098, 5400			   ; 3520Hz


;--------------------------------------------------------------------;
; Envelope Table
;--------------------------------------------------------------------;

envelope:
	.db 255,252,250,247,245,243,240,238,235,233,231,228,226,224,222,219
	.db 217,215,213,211,209,207,205,203,201,199,197,195,193,191,189,187
	.db 185,183,182,180,178,176,174,173,171,169,168,166,164,163,161,159
	.db 158,156,155,153,152,150,149,147,146,144,143,141,140,139,137,136
	.db 134,133,132,130,129,128,127,125,124,123,122,120,119,118,117,116
	.db 115,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99
	.db 98,97,96,95,94,93,92,91,90,89,88,87,87,86,85,84
	.db 83,82,82,81,80,79,78,78,77,76,75,75,74,73,72,72
	.db 71,70,69,69,68,67,67,66,65,65,64,64,63,62,62,61
	.db 60,60,59,59,58,57,57,56,56,55,55,54,54,53,53,52
	.db 51,51,50,50,49,49,48,48,48,47,47,46,46,45,45,44
	.db 44,43,43,43,42,42,41,40,40,39,39,38,38,37,37,36
	.db 35,35,34,34,33,33,32,31,31,30,30,29,29,28,28,27
	.db 26,26,25,25,24,24,23,22,22,21,21,20,20,19,19,18
	.db 17,17,16,16,15,15,14,13,13,12,12,11,11,10,10,9
	.db 8,8,7,7,6,6,5,4,4,3,3,2,2,1,1,0


;.include "wavetable_8bit_square.inc"
.include "wavetable_celesta.inc"
;.include "wavetable_flute.inc"
;.include "wavetable_piano.inc"
;.include "wavetable_string.inc"

;--------------------------------------------------------------------;
; Score table
;--------------------------------------------------------------------;
;.org 1024
score:
;.db 0,26|128,128,1,2,3,4,5|128,255,255,0,18|128,125,1,4,58|128,2,255
;.include "melody.asm"

