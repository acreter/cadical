#include "cadical.hpp"

namespace CaDiCaL {

struct Wrapper : Terminator {
  Solver * solver;
  void * state;
  int (*function) (void *);
  bool terminate () { return function ? function (state) : false; }
  Wrapper () : solver (new Solver ()), state (0), function (0) { }
  ~Wrapper () { function = 0; delete solver; }
};

}

using namespace CaDiCaL;

extern "C" {

#include "ccadical.h"
#include "csolver.h"

const char * ccadical_signature (void) {
  return Solver::signature ();
}

CCaDiCaL * ccadical_init (void) {
  return (CCaDiCaL*) new Wrapper ();
}

void ccadical_release (CCaDiCaL * wrapper) {
  delete (Wrapper*) wrapper;
}

void ccadical_set_option (CCaDiCaL * wrapper,
                          const char * name, int val) {
  ((Wrapper*) wrapper)->solver->set (name, val);
}

void ccadical_limit (CCaDiCaL * wrapper,
                     const char * name, int val) {
  ((Wrapper*) wrapper)->solver->limit (name, val);
}

int ccadical_get_option (CCaDiCaL * wrapper, const char * name) {
  return ((Wrapper*) wrapper)->solver->get (name);
}

void ccadical_add (CCaDiCaL * wrapper, int lit) {
  ((Wrapper*) wrapper)->solver->add (lit);
}

void ccadical_assume (CCaDiCaL * wrapper, int lit) {
  ((Wrapper*) wrapper)->solver->assume (lit);
}

int ccadical_solve (CCaDiCaL * wrapper) {
  return ((Wrapper*) wrapper)->solver->solve ();
}

int ccadical_simplify (CCaDiCaL * wrapper) {
  return ((Wrapper*) wrapper)->solver->simplify ();
}

int ccadical_val (CCaDiCaL * wrapper, int lit) {
  return ((Wrapper*) wrapper)->solver->val (lit);
}

int ccadical_failed (CCaDiCaL * wrapper, int lit) {
  return ((Wrapper*) wrapper)->solver->failed (lit);
}

void ccadical_print_statistics (CCaDiCaL * wrapper) {
  ((Wrapper*) wrapper)->solver->statistics ();
}

void ccadical_terminate (CCaDiCaL * wrapper) {
  ((Wrapper*) wrapper)->solver->terminate ();
}

int64_t ccadical_active (CCaDiCaL * wrapper) {
  return ((Wrapper*) wrapper)->solver->active ();
}

int64_t ccadical_irredundant (CCaDiCaL * wrapper) {
  return ((Wrapper*) wrapper)->solver->irredundant ();
}

int ccadical_fixed (CCaDiCaL * wrapper, int lit) {
  return ((Wrapper*) wrapper)->solver->fixed (lit);
}

void ccadical_set_terminate (CCaDiCaL * ptr,
                             void * state, int (*terminate)(void *)) {
  Wrapper * wrapper = (Wrapper *) ptr;
  wrapper->state = state;
  wrapper->function = terminate;
  if (terminate) wrapper->solver->connect_terminator (wrapper);
  else wrapper->solver->disconnect_terminator ();
}

void ccadical_freeze (CCaDiCaL * ptr, int lit) {
  ((Wrapper*) ptr)->solver->freeze (lit);
}

void ccadical_melt (CCaDiCaL * ptr, int lit) {
  ((Wrapper*) ptr)->solver->melt (lit);
}

int ccadical_frozen (CCaDiCaL * ptr, int lit) {
  return ((Wrapper*) ptr)->solver->frozen (lit);
}

CSolver *
csolver_init(){
  	return (CSolver*) new Wrapper ();
}

CSolver *
csolver_reset(CSolver * s){
	delete (Wrapper *) s;
  	return (CSolver*) new Wrapper ();
}

void
csolver_release(CSolver * s){
	delete (Wrapper *) s;
}

int
csolver_solve(CSolver * s){
	switch(((Wrapper*) s)->solver->solve()){
		case(10):	return 1;
		case(20):	return 0;
		default:	return -1;
	}
}

void
csolver_clause(CSolver * s, int nLits, ...){
	Wrapper * w = (Wrapper *) s;
	int lit;

	va_list args;
	va_start(args, nLits);

	for(int i = 0; i < nLits; ++i){
		if(!(lit = va_arg(args, int))){
#ifdef LOGGING
			fprintf(stderr, "WARNING: clause terminated before end of args reached\n");
#endif
		}
		w->solver->add(lit);
	}

	w->solver->add(0);

	va_end(args);
}

void
csolver_clausezt(CSolver * s, ...){
	Wrapper * w = (Wrapper *) s;
	int lit;

	va_list args;
	va_start(args, s);

	while((lit = va_arg(args, int)) != 0){
		w->solver->add(lit);
		ccadical_add((CCaDiCaL *) s, lit);
	}

	w->solver->add(0);

	va_end(args);
}

void
csolver_aclause(CSolver * s, int nLits, int * lits){
	Wrapper * w = (Wrapper *) s;
	for(int i = 0; i < nLits; ++i){
		if(!lits[i]){
#ifdef LOGGING
			fprintf(stderr, "WARNING: clause terminated before end of args reached\n");
#endif
		}
		w->solver->add(lits[i]);
	}

	w->solver->add(0);

}
void
csolver_aclausezt(CSolver * s, int * lits){
	Wrapper * w = (Wrapper *) s;
	while(*lits != 0){
		w->solver->add(*lits++);
	}

	w->solver->add(0);
}

int
csolver_val(CSolver * s, int lit){
	return ((Wrapper*) s)->solver->val(lit);
}

const struct clibrary LibCSolver = {
	.init =			csolver_init,
	.reset =		csolver_reset,
	.release =		csolver_release,
	.solve =		csolver_solve,

	.clause = 		csolver_clause,
	.clausezt =		csolver_clausezt,
	.aclause =		csolver_aclause,
	.aclausezt =	csolver_aclausezt,

	.val =			csolver_val
};
}
