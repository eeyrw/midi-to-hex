; operation definitons for dsu module
; u : unsigned
; s : signed

#ifndef	_DSU_DEF_INC_
#define	_DSU_DEF_INC_

#define	XADD		0x05	; DA = DX + DY
#define	XADS		0x25	; DA = DX + DY with signed
#define	XSUB		0x01	; DA = DX - DY
#define	XSBS		0x21	; DA = DX - DY with signed
#define	MVY			0x1d	; DA = DY
#define	MVNY		0x19	; DA = -DY
#define	MVNYS		0x39	; DA = -DY with signed
#define	ADAY		0x17	; DA = DA + DY
#define	ADAYS		0x37	; DA = DA + DY with signed
#define	SBAY		0x13	; DA = DA - DY
#define	SBAYS		0x33	; DA = DA - DY with signed

#define	XMULUU		0x44	; DA = (u)DX * (u)DY
#define	XMULUS		0x54	; DA = (u)DX * (s)DY
#define	XMULSU		0x64	; DA = (s)DX * (u)DY
#define	XMULSS		0x74	; DA = (s)DX * (s)DY

#define	XMNLUU		0x40	; DA = -(XMULUU)
#define	XMNLUS		0x50	; DA = -(XMULUS)
#define	XMNLSU		0x60	; DA = -(XMULSU)
#define	XMNLSS		0x70	; DA = -(XMULSS)

#define	FXMULUU		0x4c	; DA = (XMULUU) >> 1
#define	FXMULUS		0x5c	; DA = (XMULUS) >> 1
#define	FXMULSU		0x6c	; DA = (XMULSU) >> 1
#define	FXMULSS		0x7c	; DA = (XMULSS) >> 1

#define	FXMNLUU		0x48	; DA = (-XMULUU) >> 1
#define	FXMNLUS		0x58	; DA = (-XMULUS) >> 1
#define	FXMNLSU		0x68	; DA = (-XMULSU) >> 1
#define	FXMNLSS		0x78	; DA = (-XMULSS) >> 1

#define	XMACUU		0x46	; DA = DA + XMULUU
#define	XMACUS		0x56	; DA = DA + XMULUS
#define	XMACSU		0x66	; DA = DA + XMULSU
#define	XMACSS		0x76	; DA = DA + XMULSS

#define	SMACUU		0x47	; DA = DA + XMULUU with signed
#define	SMACUS		0x57	; DA = DA + XMULUS with signed 
#define	SMACSU		0x67	; DA = DA + XMULSU with signed
#define	SMACSS		0x77	; DA = DA + XMULSS with signed

#define	FXMACUU		0x4e	; DA = (XMACUU) >> 1
#define	FXMACUS		0x5e	; DA = (XMACUS) >> 1
#define	FXMACSU		0x6e	; DA = (XMACSU) >> 1
#define	FXMACSS		0x7e	; DA = (XMACSS) >> 1

#define	FSMACUU		0x4f	; DA = (XMACUU) >> 1 with signed
#define	FSMACUS		0x5f	; DA = (XMACUS) >> 1 with signed
#define	FSMACSU		0x6f	; DA = (XMACSU) >> 1 with signed
#define	FSMACSS		0x7f	; DA = (XMACSS) >> 1 with signed

#define	XMSCUU		0x42	; DA = DA - XMULUU
#define	XMSCUS		0x52	; DA = DA - XMULUS
#define	XMSCSU		0x62	; DA = DA - XMULSU
#define	XMSCSS		0x72	; DA = DA - XMULSS

#define	SMSCUU		0x43	; DA = DA - XMULUU with signed
#define	SMSCUS		0x53	; DA = DA - XMULUS with signed
#define	SMSCSU		0x63	; DA = DA - XMULSU with signed
#define	SMSCSS		0x73	; DA = DA - XMULSS with signed

#define	FMSCUU		0x4a	; DA = (XMSCUU) >> 1
#define	FMSCUS		0x5a	; DA = (XMSCUS) >> 1
#define	FMSCSU		0x6a	; DA = (XMSCSU) >> 1
#define	FMSCSS		0x7a	; DA = (XMSCSS) >> 1

#define	FSMSCUU		0x4b	; DA = (XMSCUU) >> 1 with signed
#define	FSMSCUS		0x5b	; DA = (XMSCUS) >> 1 with signed
#define	FSMSCSU		0x6b	; DA = (XMSCSU) >> 1 with signed
#define	FSMSCSS		0x7b	; DA = (XMSCSS) >> 1 with signed

#define	CLRA		0x80	; DA = 0
#define	NEGA		0x84	; DA = NEG(DA)
#define NEGAS		0x85	; DA = NEG(DA) with signed
#define	USQX		0x88	; DA = DX^2
#define	SSQX		0x89	; DA = DX^2 with signed
#define	USQY		0x8a	; DA = DY^2
#define SSQY		0x8b	; DA = DY^2 with signed
#define	ABSA		0xa0	; DA = ABS(DA)
;#define ABSAS		0xa1	; DA = ABS(DA) with signed
#define	XDIVA		0xb0	; DA = DA/DY
#define	XDIVR		0xb1	; DA = DA/DY; DY = DA%DY

#define	ASLA		0xc0	; DA = DA << n : ASLA | (n & 0xf) with signed or unsigned
#define ASRA		0xd0	; DA = DA >> n : ASRA | (n & 0xf) with unsigned
#define	ASRAS		0xf0	; DA = DA >> n : ASRAS | (n & 0xf) with signed

#endif