#ifndef _csolver_h_INCLUDED
#define _csolver_h_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

typedef struct CSolver CSolver;

struct clibrary{
	CSolver *	(*init)(void);
	CSolver *	(*reset)(CSolver *);
	void 		(*release)(CSolver *);
	int			(*solve)(CSolver *);

	void		(*clause)(CSolver *, int, ...);
	void		(*clausezt)(CSolver *, ...);
	void		(*aclause)(CSolver *, int, int *);
	void		(*aclausezt)(CSolver *, int *);

	int			(*val)(CSolver *, int);
};

extern const struct clibrary LibCSolver;

CSolver * csolver_init(void);
CSolver * csolver_reset(CSolver *);
void csolver_release(CSolver *);
int csolver_solve(CSolver *);

void csolver_clause(CSolver *, int, ...);
void csolver_clausezt(CSolver *, ...);
void csolver_aclause(CSolver *, int, int *);
void csolver_aclausezt(CSolver *, int *);

int csolver_val(CSolver *, int);

#ifdef __cplusplus
}
#endif
#endif
