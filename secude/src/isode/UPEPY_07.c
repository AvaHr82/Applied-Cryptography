/* automatically generated by pepy 8.0 #2 (pilsen), do not edit! */

#include "psap.h"

#define	advise	PY_advise

void	advise ();

/* Generated from module UNIV */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepsy/RCS/UNIV.py,v 9.0 1992/06/16 12:24:03 isode Rel $";
#endif

#ifndef PEPYPARM
#define PEPYPARM char *
#endif /* PEPYPARM */
extern PEPYPARM NullParm;

/* ARGSUSED */

int	build_UNIV_PrintableString (pe, explicit, len, buffer, parm)
register PE     *pe;
int	explicit;
int	len;
char   *buffer;
PEPYPARM parm;
{
    if (build_UNIV_IA5String (pe, 0, len, buffer, NullParm) == NOTOK)
        return NOTOK;
    (*pe) -> pe_class = PE_CLASS_UNIV;
    (*pe) -> pe_id = 19;

#ifdef DEBUG
    (void) testdebug ((*pe), "UNIV.PrintableString");
#endif


    return OK;
}
