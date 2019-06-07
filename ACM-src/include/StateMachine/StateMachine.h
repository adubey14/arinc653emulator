#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <deque>
#include <map>
#include <set>
#include <vector>

using namespace std;
#include <ACM/USER/ACM_TIMER.h>

namespace STATEMACHINE {

typedef unsigned long STATEID;
typedef unsigned long TRANSID;
typedef SYSTEM_TIMESPEC TIMETYPE;
typedef ACM_HELPERS::ACM_TIMER TIMERTYPE;

enum StateType { OR, AND };
enum EventType { INTERNAL, EXTERNAL, BOTH };

class Region;
class State;
class Transition;
class DataSource;
class TransitionComp;
class RegionComp;

class TransitionComp
    : public binary_function<Transition *, Transition *, bool> {
public:
  bool operator()(Transition *t1, Transition *t2) const;
};

class RegionComp : public binary_function<Region *, Region *, bool> {
public:
  bool operator()(Region *r1, Region *r2) const;
};

class State {
public:
  State(Region *pRegion, State *pState, string &name, StateType t, STATEID sid);
  void SetDataSource(DataSource *ds);
  void SetDefaultAction();
  void addOutgoingTransition(Transition *t);
  void addTimeOutTransition(Transition *t);
  void addChildRegion(Region *cRegion);

  virtual void performEntry();
  virtual void performExit();
  virtual void performDuring();

  Transition *EvalTransitions(deque<Transition *> &activeTransitions,
                              set<TRANSID> &timeOutsToEval,
                              int &defaultStateActionID,
                              bool lookForDefaultStateAction = true);
  void EvalTransitionsForTimeOut(bool evalduring = true);

  void EvalExit(set<TRANSID> &setTimeOutTransIDs, bool doExit = true);
  void EvalTimeOut(deque<State *> &statesToEnter, bool applyHistory,
                   bool doEntry = true);
  void EvalEntry(deque<State *> &statesToEnter, bool applyHistory,
                 bool doEntry = true);

  void computePathToRoot();

  Region *m_pRegion;
  State *m_pState;
  StateType m_type;
  string m_name;
  STATEID m_id;
  DataSource *m_ds;
  bool m_DefaultActionSet;

  set<Transition *, TransitionComp> m_setOrderedOutTransitions,
      m_setTimeOutTransitions;
  set<Region *, RegionComp> m_setOrderedRegions;

  deque<Region *> m_RegionPathToRoot;
  deque<State *> m_StatePathToRoot;
};

class Region {
public:
  Region(string &name, int priorityID = 0, State *pState = 0);
  void addState(State *state, bool isInitial = false);
  void setCurrentState(State *state);

  void EnableShallowHistory();
  void EnableDeepHistory();
  void EvalExit(set<TRANSID> &setTimeOutTransIDs);
  void EvalTimeOut(deque<State *> &statesToEnter, bool applyHistory,
                   bool checkHistory = false, bool doEntry = true);
  void EvalEntry(deque<State *> &statesToEnter, bool applyHistory,
                 bool checkHistory = false);

  State *m_pState;
  string m_name;
  int m_priority;

  State *m_initState; // stores init state or history (if history is enabled)
  State *m_currentState;
  bool m_historyEnabled, m_isDeepHistory; // default false;
  set<State *> m_childStates;
};

class Transition {
public:
  Transition(State *source, State *target, int priorityID, TRANSID id,
             bool setHistory = false);

  void SetDataSource(DataSource *ds);

  void SetEvalTimeOut(bool);

  virtual bool checkEvent();
  virtual bool checkGuard();
  virtual void performAction();

  void computeEntryAndExitPoints();

  State *m_SourceState, *m_TargetState;
  int m_priority;
  TRANSID m_id;
  DataSource *m_ds;
  bool m_TransitionBetweenParallelMachines;
  bool m_setHistory;
  bool m_evalTimeOut;

  State *m_StateToExit;
  vector<State *> m_StatesToEnter;
  vector<Region *> m_RegionsToExit, m_RegionsToEnter;
};

class Event {
public:
  Event();
  Event(string name, int id);
  bool operator<(const Event &rhs) const;
  string m_name;
  int m_id;
};

class EventComp : public binary_function<Event, Event, bool> {
public:
  bool operator()(Event &e1, Event &e2) const;
};

class DataSource {

public:
  DataSource() {}
  virtual ~DataSource() {}

  virtual void ResetEvents() = 0; // reset input,output,internal event list
  virtual void ApplyAndResetInputEvents() = 0; // Resets current events. Apply
                                               // input events as current event
                                               // and reset input event list
  virtual void ApplyAndResetInternalEvents() = 0; // Resets current events.
                                                  // Apply internal events as
                                                  // current event and reset
                                                  // input event list
  virtual void ResetOutputEvents() = 0;   // Resets output event list
  virtual void ResetInputEvents() = 0;    // Resets input event list
  virtual void ResetInternalEvents() = 0; // Resets internal event list
  virtual bool
  IsCurrentEventSet() = 0; // Checks if there are any internal events set
  virtual void
  GetOutputEvents(map<string, set<Event *>>
                      &events) = 0; // Gets the list of Output Events generated

  virtual void
  performStateEntryAction(STATEID sid) = 0; // Execute state entry-action
  virtual void
  performStateExitAction(STATEID sid) = 0; // Execute state exit-action
  virtual void
  performStateDuringAction(STATEID sid) = 0; // Execute state during-action
  virtual void performDefaultStateAction(STATEID sid) = 0; // Execute default
                                                           // action from a
                                                           // state when no
                                                           // outgoing
                                                           // transitions are
                                                           // active

  virtual void
  checkAndPerformDefaultEventAction() = 0; // Execute default event action

  virtual void
  performTransitionAction(TRANSID tid) = 0; // Execute transaction -action
  virtual bool
  evalTransitionGuard(TRANSID tid) = 0; // Evaluate transaction - guard
  virtual bool
  evalTransitionEventTrigger(TRANSID tid) = 0; // Evaluate transaction - trigger

  virtual void setCurrentTime() = 0; // set current time
  virtual void clearTimeOutCalls(
      set<TRANSID> &setTimeOutTransIDs) = 0; // clear any pending timeout calls
  virtual void EvaluateTimeOuts() = 0;       // evaluate new time-out calls
  virtual void GetTimeOutTransitionsToEvaluate(
      set<TRANSID>
          &timeOutTransitions) = 0; // get the list of time out transitions

  bool m_EvaluatingTimeOuts;
};

class StateMachine : public State {
public:
  StateMachine(string &name, DataSource *ds); // create state with null
                                              // parent(region and state) and
                                              // sets the statetype to OR

  void SetEvent(Event e);

  void start();

  void step(bool init = false);

  DataSource *m_ds;

  ACM_INTERNAL::MUTEX m_StateMutex;
};
}

#endif // STATE_MACHINE_H
