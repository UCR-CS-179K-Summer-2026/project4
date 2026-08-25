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

//------SHOULD NOT DETECT, there is no inheritance
class NoInheritance {
public:
    void doSomething();
};

//----------------------------

//------SHOULD NOT DETECT, uses base class member function helper on derived class object u-----
class Unused2Inheritance : public Base {
public:
    void doOwnThing() {
        int local = 5;
    }
};

void test(Unused2Inheritance u){
    u.doOwnThing();
    u.helper();
}

//----------------------------

//------SHOULD NOT DETECT, subclass does not explicitly use baseclass but the object calls baseclass member------
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
    android.takeStep();
}

//----------------------------

//------SHOULD NOT DETECT, bird doesn't 'use' its baseClass but external function named useBirdViaAssignment() does------
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

//------SHOULD NOT DETECT: Use copy initialization at declaration time------
class Canine {
    public:
    int food:
};

class Dog : public Canine{};

void useDogViaCopyInit(){
    Dog d;
    Canine c = d;
}

//----------------------------

//------SHOULD DETECT: used externally, compiler assumes that ShadowRobot redeclares step()------
class Mover {
public:
    void step();
};
 
class ShadowRobot : public Mover {
public:
    void step();   
};
 
void useShadowRobotExternally() {
    ShadowRobot r;
    r.step();
}
//----------------------------

//-------SHOULD DETECT: Hybrid inherits from both bases but dont actually use them-------
class Flyable {
public:
    void fly();
};
 
class Swimmable {
public:
    void swim();
};
 
class Hybrid : public Flyable, public Swimmable {
public:
    void doOwnThing() {
        int x = 1;
    }
};
//----------------------------

//------SHOULD NOT DETECT: ReadWriter inherits from both baseclasses but also uses both------
class Reader {
public:
    void read();
};
 
class Writer {
public:
    void write();
};
 
class ReadWriter : public Reader, public Writer {
public:
    void doBoth() {
        Reader::read();
        Writer::write();
    }
};
//----------------------------

//SHOULD DETECT: the base class Spirit is not defined anywhere in the test file, not a valid inheritance
class Ghost : public Spirit {
    public:
    void doOwnThing(){
        int x = 1;
    }

};
//----------------------------

//SHOULD NOT DETECT: Valid inheritance since the derived class Car uses the member of its base class properly
class Engine{
    public:
    void start();
};

class Car : private Engine {
    public:
    void drive{
        Engine::start();
    }
};

//----------------------------
