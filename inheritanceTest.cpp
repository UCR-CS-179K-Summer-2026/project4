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

//SHould be skipped as there is no inheritance
class NoInheritance {
public:
    void doSomething();
};
