#include "crcmodel.h"
#define BITMASK(X) (1L << (X))
#define MASK32 0xFFFFFFFFL
#define LOCAL static
LOCAL ulong reflect P_((ulong v,int b));
LOCAL ulong reflect (ulong v,int b) {

	int i;
	ulong t = v;
	for (i=0; i<b; i++)
	{
		if (t & 1L)
			v|= BITMASK((b-1)-i);
		else
			v&= ~BITMASK((b-1)-i);
		t>>=1;
	}
	return v;
}

LOCAL ulong widmask P_((p_cm_t));

LOCAL ulong widmask (p_cm_t p_cm)
{
	return (((1L<<(p_cm->cm_width-1))-1L)<<1)|1L;
}

void cm_ini (p_cm_t p_cm)
{
	p_cm->cm_reg = p_cm->cm_init;
}

void cm_nxt (p_cm_t p_cm,int ch)
{
	int i;
	ulong uch = (ulong) ch;
	ulong topbit = BITMASK(p_cm->cm_width-1);
	if (p_cm->cm_refin) uch = reflect(uch,8);
	p_cm->cm_reg ^= (uch << (p_cm->cm_width-8));
	for (i=0; i<8; i++)
	{
		if (p_cm->cm_reg & topbit)
			p_cm->cm_reg = (p_cm->cm_reg << 1) ^ p_cm->cm_poly;
		else
			p_cm->cm_reg <<= 1;
		p_cm->cm_reg &= widmask(p_cm);
	}
}

void cm_blk (p_cm_t p_cm,p_ubyte_ blk_adr,ulong blk_len)
{
	while (blk_len--) cm_nxt(p_cm,*blk_adr++);
}

ulong cm_crc (p_cm_t p_cm)
{
	if (p_cm->cm_refot)
		return p_cm->cm_xorot ^ reflect(p_cm->cm_reg,p_cm->cm_width);
	else
		return p_cm->cm_xorot ^ p_cm->cm_reg;
}

ulong cm_tab (p_cm_t p_cm,int index)
{
	int i;
	ulong r;
	ulong topbit = BITMASK(p_cm->cm_width-1);
	ulong inbyte = (ulong) index;
	if (p_cm->cm_refin) inbyte = reflect(inbyte,8);
	r = inbyte << (p_cm->cm_width-8);
	for (i=0; i<8; i++)
		if (r & topbit)
			r = (r << 1) ^ p_cm->cm_poly;
		else
			r<<=1;
		if (p_cm->cm_refin) r = reflect(r,p_cm->cm_width);
			return r & widmask(p_cm);
}
