//------SHOULD NOT DETECT as each subclass inherits and uses something from parent class-----
class Base {
public:
    virtual void greet();
    void helper();
    int baseField;
};

class UsesBaseQualified : public Base {
public:
    void greet() override {
        Base::greet();
    }
};

class UsesBaseConstructor : public Base {
public:
    UsesBaseConstructor(int x) : Base(), memberField(x) {}
    int memberField;
};

class UsesBaseOverride : public Base {
public:
    void greet() override {
        // does its own thing, never calls Base::greet() directly
        int local = 42;
    }
};
//----------------------------

//------SHOULD DETECT as it inherits Base but doesnt use anything from it-----
class UnusedInheritance : public Base {
public:
    void doOwnThing() {
        int local = 5;
    }
};

//----------------------------

//SHOULD NOT DETECT, there is no inheritance
class NoInheritance {
public:
    void doSomething();
};

//----------------------------

//------SHOULD NOT DETECT BUT DETECTS ANYWAY BUGGED!-----
class UnusedInheritance : public Base {
public:
    void doOwnThing() {
        int local = 5;
    }
};

void test(UnusedInheritance u){
    u.doOwnThing();
    u.helper();
}

//----------------------------

//------SHOULD NOT DETECT, subclass does not explicitly use baseclass but the object calls baseclass member
class Walker{
    public: 
        void takeStep();
};

class Robot: public Walker {
    public:
        void speak();
};

void useRobotExternally(){
    Robot android;
    // android.takeStep();
}

//----------------------------

//------SHOULD NOT DETECT, bird doesn't 'use' its baseClass but external function named useBirdViaAssignment() does
class Creature{
    public:
        int speed;
};

class Bird : public Creature{};

void useBirdViaAssignment() {
    Bird b;
    Creature creatureVar;
    creatureVar = b;
}

//----------------------------
