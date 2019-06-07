#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <StateMachine/StateMachine.h>
using namespace std;
using namespace STATEMACHINE;

class Test1DataSource : public STATEMACHINE::DataSource {
public:
  Test1DataSource() {}
  virtual ~Test1DataSource() {}

  virtual void ResetEvents() // reset input,output,internal event list
  {}
  virtual void ApplyAndResetInputEvents() // Resets current events. Apply input
                                          // events as current event and reset
                                          // input event list
  {}
  virtual void ApplyAndResetInternalEvents() // Resets current events. Apply
                                             // internal events as current event
                                             // and reset input event list
  {}

  virtual void ResetOutputEvents() // Resets output event list
  {}
  virtual bool IsInternalEventSet() {
    return false;
  } // Checks if there are any internal events set
  virtual void GetOutputEvents(set<Event> &events) {
    events.clear();
  } // Gets the list of Output Events generated

  virtual void performStateEntryAction(STATEID sid) {
  } // Execute state entry-action
  virtual void performStateExitAction(STATEID sid) {
  } // Execute state exit-action
  virtual void performStateDuringAction(STATEID sid) {
  } // Execute state during-action

  virtual void performTransitionAction(TRANSID tid) {
  }                                             // Execute transaction -action
  virtual bool evalTransitionGuard(TRANSID tid) // Evaluate transaction - guard
  {
    return true;
  }
  virtual bool
  evalTransitionEventTrigger(TRANSID tid) // Evaluate transaction - trigger
  {
    return true;
  }
};

int mainTest1() {
  Test1DataSource *ds = new Test1DataSource();

  string smname("STest1");
  StateMachine sm(smname, ds);

  string smrname("RSTest1");
  Region smr(smrname, 0, &sm);
  sm.addChildRegion(&smr);

  string s1name("s1");
  State s1(&smr, &sm, s1name, STATEMACHINE::OR, 1);
  smr.addState(&s1, true);

  string s2name("s2");
  State s2(&smr, &sm, s2name, STATEMACHINE::OR, 2);
  smr.addState(&s2, false);

  Transition t1(&s1, &s2, 1, 1);
  s1.addOutgoingTransition(&t1);

  Transition t2(&s2, &s1, 2, 2);
  s2.addOutgoingTransition(&t2);

  s1.SetDataSource(ds);
  s2.SetDataSource(ds);
  t1.SetDataSource(ds);
  t2.SetDataSource(ds);

  sm.computePathToRoot();
  s1.computePathToRoot();
  s2.computePathToRoot();

  t1.computeEntryAndExitPoints();
  t2.computeEntryAndExitPoints();

  sm.step();
  cout << "------------------------------------" << endl;
  sm.step();

  return 0;
}

namespace MACHINE1 {

Event TICK("TICK", 1);
Event RESET("RESET", 2);

class DataSource : public STATEMACHINE::DataSource {
public:
  DataSource() {
    m_inputEvents.clear();
    m_outputEvents.clear();
    m_internalEvents.clear();
    m_currentEvents.clear();

    counter = 0;
  }
  virtual ~DataSource() {
    m_inputEvents.clear();
    m_outputEvents.clear();
    m_internalEvents.clear();
    m_currentEvents.clear();
  }

  virtual void ResetEvents() // reset input,output,internal event list
  {
    m_inputEvents.clear();
    m_outputEvents.clear();
    m_internalEvents.clear();
    m_currentEvents.clear();
  }
  virtual void ApplyAndResetInputEvents() // Resets current events. Apply input
                                          // events as current event and reset
                                          // input event list
  {
    m_currentEvents.clear();
    std::copy(m_inputEvents.begin(), m_inputEvents.end(),
              std::inserter(m_currentEvents, m_currentEvents.begin()));
    m_inputEvents.clear();
  }
  virtual void ApplyAndResetInternalEvents() // Resets current events. Apply
                                             // internal events as current event
                                             // and reset input event list
  {
    m_currentEvents.clear();
    std::copy(m_internalEvents.begin(), m_internalEvents.end(),
              std::inserter(m_currentEvents, m_currentEvents.begin()));
    m_internalEvents.clear();
  }

  virtual void ResetOutputEvents() // Resets output event list
  {
    m_outputEvents.clear();
  }

  virtual bool IsInternalEventSet() {
    return !m_internalEvents.empty();
  } // Checks if there are any internal events set

  virtual void GetOutputEvents(set<Event> &events) {
    events.clear();
    std::copy(m_outputEvents.begin(), m_outputEvents.end(),
              std::inserter(events, events.begin()));

  } // Gets the list of Output Events generated

  virtual void performStateEntryAction(STATEID sid) {
  } // Execute state entry-action
  virtual void performStateExitAction(STATEID sid) {
  } // Execute state exit-action
  virtual void performStateDuringAction(STATEID sid) {
  } // Execute state during-action

  virtual void performTransitionAction(TRANSID tid) {
    switch (tid) {
    case 1:
      return TA_T1();
    case 2:
      return TA_T2();
    default:
      return;
    }
    return;
  } // Execute transaction -action

  virtual bool evalTransitionGuard(TRANSID tid) // Evaluate transaction - guard
  {
    return true;
  }

  virtual bool
  evalTransitionEventTrigger(TRANSID tid) // Evaluate transaction - trigger
  {
    switch (tid) {
    case 1:
      return ET_T1();
    case 2:
      return ET_T2();
    default:
      break;
    }
    return true;
  }

  bool ET_T1() {
    return (m_currentEvents.find(RESET) != m_currentEvents.end());
  }

  bool ET_T2() { return (m_currentEvents.find(TICK) != m_currentEvents.end()); }

  void TA_T1() { counter = 0; }

  void TA_T2() { counter++; }

  void SetEvent(Event e) { m_inputEvents.insert(e); }

  void Send(Event e) { m_outputEvents.insert(e); }

  void Consume(Event e) { m_internalEvents.insert(e); }

  set<Event> m_inputEvents;
  set<Event> m_outputEvents;
  set<Event> m_internalEvents;
  set<Event> m_currentEvents;

  int counter;
};
}

int mainTest2() {
  MACHINE1::DataSource *ds = new MACHINE1::DataSource();

  string smname("Machine1");
  StateMachine sm(smname, ds);

  string smrname("RMachine1");
  Region smr(smrname, 0, &sm);
  sm.addChildRegion(&smr);

  string sNominalname("Nominal");
  State sNominal(&smr, &sm, sNominalname, STATEMACHINE::OR, 1);
  smr.addState(&sNominal, true);

  Transition t1(&sNominal, &sNominal, 1, 1);
  sNominal.addOutgoingTransition(&t1);

  Transition t2(&sNominal, &sNominal, 2, 2);
  sNominal.addOutgoingTransition(&t2);

  sNominal.SetDataSource(ds);
  t1.SetDataSource(ds);
  t2.SetDataSource(ds);

  sm.computePathToRoot();
  sNominal.computePathToRoot();

  t1.computeEntryAndExitPoints();
  t2.computeEntryAndExitPoints();

  cout << "-----------------------------------------" << endl;
  ds->SetEvent(MACHINE1::TICK);
  sm.step();
  cout << "after step1 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;
  // ds->SetEvent(MACHINE1::TICK);
  sm.step();
  cout << "after step 2 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;
  ds->SetEvent(MACHINE1::TICK);
  sm.step();
  cout << "after step 3 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;
  ds->SetEvent(MACHINE1::TICK);
  sm.step();
  cout << "after step 4 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;

  ds->SetEvent(MACHINE1::RESET);
  sm.step();
  cout << "after step 5 counter " << ds->counter << endl;

  cout << "-----------------------------------------" << endl;
  // ds->SetEvent(MACHINE1::TICK);
  sm.step();
  cout << "after step 6 counter " << ds->counter << endl;

  cout << "-----------------------------------------" << endl;
  ds->SetEvent(MACHINE1::TICK);
  sm.step();
  cout << "after step 7 counter " << ds->counter << endl;

  cout << "------------------------------------" << endl;
  // sm.step();

  return 0;
}

namespace MACHINE2 {
Event TICK("TICK", 1);
Event RESET("RESET", 2);
Event STOP("STOP", 3);
Event START("START", 4);

class DataSource : public STATEMACHINE::DataSource {
public:
  DataSource() {
    m_inputEvents.clear();
    m_outputEvents.clear();
    m_internalEvents.clear();
    m_currentEvents.clear();

    counter = 0;
  }

  virtual ~DataSource() {
    m_inputEvents.clear();
    m_outputEvents.clear();
    m_internalEvents.clear();
    m_currentEvents.clear();
  }

  virtual void ResetEvents() // reset input,output,internal event list
  {
    m_inputEvents.clear();
    m_outputEvents.clear();
    m_internalEvents.clear();
    m_currentEvents.clear();
  }

  virtual void ApplyAndResetInputEvents() // Resets current events. Apply input
                                          // events as current event and reset
                                          // input event list
  {
    m_currentEvents.clear();
    std::copy(m_inputEvents.begin(), m_inputEvents.end(),
              std::inserter(m_currentEvents, m_currentEvents.begin()));
    m_inputEvents.clear();
  }

  virtual void ApplyAndResetInternalEvents() // Resets current events. Apply
                                             // internal events as current event
                                             // and reset input event list
  {
    m_currentEvents.clear();
    std::copy(m_internalEvents.begin(), m_internalEvents.end(),
              std::inserter(m_currentEvents, m_currentEvents.begin()));
    m_internalEvents.clear();
  }

  virtual void ResetOutputEvents() // Resets output event list
  {
    m_outputEvents.clear();
  }

  virtual bool IsInternalEventSet() {
    return !m_internalEvents.empty();
  } // Checks if there are any internal events set

  virtual void GetOutputEvents(set<Event> &events) {
    events.clear();
    std::copy(m_outputEvents.begin(), m_outputEvents.end(),
              std::inserter(events, events.begin()));

  } // Gets the list of Output Events generated

  virtual void performStateEntryAction(STATEID sid) {
  } // Execute state entry-action
  virtual void performStateExitAction(STATEID sid) {
  } // Execute state exit-action
  virtual void performStateDuringAction(STATEID sid) {
  } // Execute state during-action

  virtual void performTransitionAction(TRANSID tid) {
    switch (tid) {
    case 1:
      return TA_T1();
    case 3:
      return TA_T3();
    case 4:
      return TA_T4();
    case 6:
      return TA_T6();

    default:
      return;
    }
    return;
  } // Execute transaction -action

  virtual bool evalTransitionGuard(TRANSID tid) // Evaluate transaction - guard
  {
    return true;
  }

  virtual bool
  evalTransitionEventTrigger(TRANSID tid) // Evaluate transaction - trigger
  {
    switch (tid) {
    case 1:
      return ET_T1();
    case 2:
      return ET_T2();
    case 3:
      return ET_T3();
    case 4:
      return ET_T4();
    case 5:
      return ET_T5();
    case 6:
      return ET_T6();
    default:
      break;
    }
    return true;
  }

  bool ET_T1() {
    return (m_currentEvents.find(RESET) != m_currentEvents.end());
  }

  bool ET_T2() { return (m_currentEvents.find(STOP) != m_currentEvents.end()); }

  bool ET_T3() {
    return (m_currentEvents.find(START) != m_currentEvents.end());
  }

  bool ET_T4() { return (m_currentEvents.find(TICK) != m_currentEvents.end()); }

  bool ET_T5() {
    return (m_currentEvents.find(START) != m_currentEvents.end());
  }

  bool ET_T6() { return (m_currentEvents.find(TICK) != m_currentEvents.end()); }

  void TA_T1() { counter = 0; }

  void TA_T3() { Produce(MACHINE2::START); }

  void TA_T4() { counter++; }

  void TA_T6() { counter++; }

  void SetEvent(Event e) { m_inputEvents.insert(e); }

  void Send(Event e) { m_outputEvents.insert(e); }

  void Produce(Event e) { m_internalEvents.insert(e); }

  set<Event> m_inputEvents;
  set<Event> m_outputEvents;
  set<Event> m_internalEvents;
  set<Event> m_currentEvents;

  int counter;
};
}

int mainTest3() {
  MACHINE2::DataSource *ds = new MACHINE2::DataSource();

  string smname("Machine2");
  StateMachine sm(smname, ds);

  string smrname("RMachine2");
  Region smr(smrname, 0, &sm);
  sm.addChildRegion(&smr);

  string sNominalname("Nominal");
  State sNominal(&smr, &sm, sNominalname, STATEMACHINE::AND, 1);
  smr.addState(&sNominal, true);

  Transition t1(&sNominal, &sNominal, 1, 1, true);
  sNominal.addOutgoingTransition(&t1);

  Transition t2(&sNominal, &sNominal, 2, 2);
  sNominal.addOutgoingTransition(&t2);

  string rNominal_Firstname("RNominal_First");
  Region rNominal_First(rNominal_Firstname, 0, &sNominal);
  rNominal_First.EnableDeepHistory();
  sNominal.addChildRegion(&rNominal_First);

  string sFirstname("First");
  State sFirst(&rNominal_First, &sNominal, sFirstname, STATEMACHINE::OR, 2);
  rNominal_First.addState(&sFirst, true);

  string rNominal_Secondname("RSecond");
  Region rNominal_Second(rNominal_Secondname, 1, &sNominal);
  rNominal_Second.EnableDeepHistory();
  sNominal.addChildRegion(&rNominal_Second);
  string sSecondname("Second");
  State sSecond(&rNominal_Second, &sNominal, sSecondname, STATEMACHINE::OR, 3);
  rNominal_Second.addState(&sSecond, true);

  string rFirstname("RFirst");
  Region rFirst(rFirstname, 0, &sFirst);
  sFirst.addChildRegion(&rFirst);

  string sOffname("Off");
  State sOff(&rFirst, &sFirst, sOffname, STATEMACHINE::OR, 4);
  rFirst.addState(&sOff, true);

  string sOnname("On");
  State sOn(&rFirst, &sFirst, sOnname, STATEMACHINE::OR, 5);
  rFirst.addState(&sOn, false);

  Transition t3(&sOff, &sOn, 1, 3);
  sOff.addOutgoingTransition(&t3);

  Transition t4(&sOn, &sOn, 1, 4);
  sOn.addOutgoingTransition(&t4);

  string rSecondname("RSecond");
  Region rSecond(rSecondname, 0, &sSecond);
  sSecond.addChildRegion(&rSecond);

  State sOff1(&rSecond, &sSecond, sOffname, STATEMACHINE::OR, 6);
  rSecond.addState(&sOff1, true);

  State sOn1(&rSecond, &sSecond, sOnname, STATEMACHINE::OR, 7);
  rSecond.addState(&sOn1, false);

  Transition t5(&sOff1, &sOn1, 1, 5);
  sOff1.addOutgoingTransition(&t5);

  Transition t6(&sOn, &sOn, 1, 6);
  sOn1.addOutgoingTransition(&t6);

  sm.SetDataSource(ds);
  sNominal.SetDataSource(ds);
  t1.SetDataSource(ds);
  t2.SetDataSource(ds);

  sFirst.SetDataSource(ds);
  sOff.SetDataSource(ds);
  sOn.SetDataSource(ds);
  t3.SetDataSource(ds);
  t4.SetDataSource(ds);

  sSecond.SetDataSource(ds);
  sOff1.SetDataSource(ds);
  sOn1.SetDataSource(ds);
  t5.SetDataSource(ds);
  t6.SetDataSource(ds);

  sm.computePathToRoot();
  sNominal.computePathToRoot();
  sFirst.computePathToRoot();
  sSecond.computePathToRoot();
  sOn.computePathToRoot();
  sOff.computePathToRoot();
  sOn1.computePathToRoot();
  sOff1.computePathToRoot();

  t1.computeEntryAndExitPoints();
  t2.computeEntryAndExitPoints();
  t3.computeEntryAndExitPoints();
  t4.computeEntryAndExitPoints();
  t5.computeEntryAndExitPoints();
  t6.computeEntryAndExitPoints();

  cout << "-----------------------------------------" << endl;
  ds->SetEvent(MACHINE2::TICK);
  sm.step();
  cout << "after step1 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;

  ds->SetEvent(MACHINE2::START);
  sm.step();
  cout << "after step 2 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;

  ds->SetEvent(MACHINE2::TICK);
  sm.step();
  cout << "after step 3 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;

  ds->SetEvent(MACHINE2::TICK);
  sm.step();
  cout << "after step 4 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;

  ds->SetEvent(MACHINE2::RESET);
  sm.step();
  cout << "after step 5 counter " << ds->counter << endl;
  cout << "-----------------------------------------" << endl;

  ds->SetEvent(MACHINE2::TICK);
  sm.step();
  cout << "after step 6 counter " << ds->counter << endl;
  cout << "------------------------------------" << endl;

  ds->SetEvent(MACHINE2::TICK);
  sm.step();
  cout << "after step 7 counter " << ds->counter << endl;

  cout << "------------------------------------" << endl;
  ds->SetEvent(MACHINE2::STOP);
  sm.step();
  cout << "after step 8 counter " << ds->counter << endl;

  cout << "------------------------------------" << endl;
  ds->SetEvent(MACHINE2::TICK);
  sm.step();
  cout << "after step 9 counter " << ds->counter << endl;

  return 0;
}
int main() {
  // return mainTest1();
  // return mainTest2();
  return mainTest3();
}