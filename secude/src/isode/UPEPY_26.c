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

int	parse_UNIV_UTCTime (pe, explicit, len, buffer, parm)
register PE	pe;
int	explicit;
int    *len;
char  **buffer;
PEPYPARM parm;
{
#ifdef DEBUG
    (void) testdebug (pe, "UNIV.UTCTime");
#endif

    if (explicit
            && PE_ID (pe -> pe_class, pe -> pe_id)
                    != PE_ID (PE_CLASS_UNIV, 23)) {
        advise (NULLCP, "UTCTime %s%s/0x%x", PEPY_ERR_BAD_CLASS,
                pe_classlist[pe -> pe_class], pe -> pe_id);
        return NOTOK;
    }
    if (parse_UNIV_VisibleString (pe, 0, len, buffer, NullParm) == NOTOK)
        return NOTOK;

    return OK;
}
