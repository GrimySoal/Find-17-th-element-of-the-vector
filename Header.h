#pragma once
#include<vector>
#include<iostream>

class Strategy {
public:
	virtual ~Strategy() {}
	virtual int use(int& current, const std::vector<int>& vect) const = 0;
};

class EveryEl : public Strategy {
private:
	EveryEl(){}
	EveryEl(const EveryEl& obj) = delete;
	EveryEl& operator=(const EveryEl& obj) = delete;
public:
	static EveryEl& getObj() {
		static EveryEl instance;
		return instance;
	}
	int use(int& current, const std::vector<int>& vect) const override {
		return vect[++current];
	}
};

class ThroughOneEl : public Strategy {
private:
	ThroughOneEl(){}
	ThroughOneEl(const ThroughOneEl& obj) = delete;
	ThroughOneEl& operator=(const ThroughOneEl& obj) = delete;
public:
	static ThroughOneEl& getObj() {
		static ThroughOneEl instance;
		return instance;
	}
	int use(int& current, const std::vector<int>& vect) const override {
		return vect[current += 2];
	}
};

class IController {
public:
	virtual void cout17El() = 0;
	virtual void activateView() const = 0;
	virtual void sendElToView(int El) const = 0;
};

class Model {
private:
	std::vector<int> vector_;
	IController* controller_;
public:
	Model(){}
	void setVector(const std::vector<int>& vector) {
		vector_ = vector;
	}
	void setController(IController*controller) {
		controller_ = controller;
	}
	auto createIterator() const {
		Iterator iter(this);
		return iter;
	}
	class Iterator;

	class Memento {
	public:
		Memento(int current, Strategy* str, Iterator* next, int size) {
			current_ = current;
			str_ = str;
			next_ = next;
			size_ = size;
		}
	private:
		friend class Iterator;
		int current_;
		Strategy* str_;
		Iterator* next_;
		int size_;
	};

	class Iterator {
	public:
		Iterator(const Model* model) {
			model_ = model;
			current_ = 0;
			str_ = nullptr;
			next_ = new IteratorVectSizeLessThan10(model);
			size_ = -1;
		}
		int getNext() {
			int a = (model_->vector_).size();
			if (a > size_) {
				next_->setStrategy(str_);
				int ret = next_->getNext();
				current_ = next_->current_;
				return ret;
			}
			else {
				return str_->use(current_, model_->vector_);
			}
		}
		void give17El() {
			this->setStrategy(&ThroughOneEl::getObj());
			int el;
			while (current_ <= 15) {
				el = getNext();
				if (current_ % 2 == 0) createMemento();
			}
			if (current_ == 16) {
				this->setStrategy(&EveryEl::getObj());
				el = getNext();
			}
			model_->controller_->activateView();
			model_->controller_->sendElToView(el);
		}
		void setStrategy(Strategy* str) {
			str_ = str;
		}
		void createMemento() {
			mems.push_back(new Memento(current_, str_, next_, size_));
		}
		void reinstateMemento(){
			auto mem = mems[mems.size() - 1];
			current_ = mem->current_;
			str_ = mem->str_;
			next_ = mem->next_;
			size_ = mem->size_;
		}
		Iterator() {}
	protected:
		const Model* model_;
		int current_;
		Strategy* str_;
		Iterator* next_;
		int size_;
		std::vector<Memento*> mems;
	};
	class IteratorVectSizeLessThan10 : public Iterator {
	public:
		IteratorVectSizeLessThan10(const Model* model) {
			model_ = model;
			current_ = 0;
			str_ = nullptr;
			next_ = new IteratorVectSizeAnyNumber(model);
			size_ = 10;
		}
	};
	class IteratorVectSizeAnyNumber : public Iterator {
	public:
		IteratorVectSizeAnyNumber(const Model* model) {
			model_ = model;
			current_ = 9;
			str_ = nullptr;
			next_ = nullptr;
			size_ = 2147483646;
		}
	};
};

class ModelBuilder {
private:
	Model* model_;
public:
	ModelBuilder(Model* model, const std::vector<int>& vector, IController* controller) {
		model_ = model;
		model_->setVector(vector);
		model_->setController(controller);
	}
};

class Observer {
public:
	virtual Model::Iterator handleEvent(Model* model, Model::Iterator* iter) = 0;
};

class CreateIterator : public Observer {
public:
	Model::Iterator handleEvent(Model* model, Model::Iterator* iter) override {
		return model->createIterator();
	}
};

class SetIteratorStrategy : public Observer {
public:
	Model::Iterator handleEvent(Model* model, Model::Iterator* iter) override {
		iter->setStrategy(&EveryEl::getObj());
		return *iter;
	}
};

class Delegation {
public:
	virtual void doStuff(Model::Iterator* it) = 0;
	virtual ~Delegation(){}
	Delegation(){}
};

class UseItMethod : public Delegation {
public:
	void doStuff(Model::Iterator* it) override {
		it->give17El();
	}
};

class DoNothing : public Delegation {
public:
	void doStuff(Model::Iterator* it) override {
		std::cout << "I've done nothing, uups" << std::endl;
	}
};

class DoStuff : public Delegation {
private:
	Delegation* cur_way;
public:
	DoStuff() {
		cur_way = new DoNothing;
	}
	void toUsItWay() {
		delete cur_way;
		cur_way = new UseItMethod;
	}
	void toDoNothing() {
		delete cur_way;
		cur_way = new DoNothing;
	}
	void doStuff(Model::Iterator* it) override {
		cur_way->doStuff(it);
	}
};

class Command {
protected:
	Model* model_;
public:
	virtual ~Command() {}
	virtual void notify() = 0;
	void setModel(Model* model) {
		model_ = model;
	}
};

class Cout17El : public Command {
private:
	std::vector<Observer*> observers_;
public:
	Cout17El() {
		observers_.push_back(new CreateIterator);
		observers_.push_back(new SetIteratorStrategy);
	}
	void notify() override {
		Model::Iterator* p = nullptr;
		for (int i = 0; i < observers_.size(); ++i) {
			p = &(observers_[i]->handleEvent(model_, p));
		}
		DoStuff obj;
		obj.toUsItWay();
		obj.doStuff(p);
	}
};

class Visitor {
public:
	virtual void visit(int El) const = 0;
};

class VisitToCout : public Visitor {
public:
	void visit(int El) const override {
		std::cout << El << std::endl;
	}
};

class Decorator : public Visitor {
private:
	Visitor* next_;
public:
	Decorator(Visitor* next) {
		next_ = next;
	}
	void visit(int El) const override {
		std::cout << "The 17-th element of the vector: ";
		next_->visit(El);
	}
};

class State {
public:
	virtual State* activateView() const = 0;
	virtual void coutEl(int El) const = 0;
};

class ViewOn : public State {
public:
	void accept(Visitor& v, int El) const {
		v.visit(El);
	}
	State* activateView() const override {
		return new ViewOn;
	}
	void coutEl(int El) const override {
		Decorator obj(new VisitToCout);
		this->accept(obj, El);
	}
};

class ViewOff : public State {
public:
	State* activateView() const override {
		return new ViewOn;
	}
	void coutEl(int El) const override {
		std::cout << "View is off :(" << std::endl;
	}
};

class View {
private:
	State* state_;
public:
	View() {
		state_ = new ViewOff;
	}
	void activateView() {
		auto t = state_->activateView();
		delete state_;
		state_ = t;
	}
	void coutEl(int El) {
		state_->coutEl(El);
	}
};

class Controller : public IController {//Который ещё и медиатор, который ещё и фасад ¯\_(ツ)_/¯
private:
	View* view_;
	Model* model_;
	std::vector<Command*> done_commands_;
	Command* cur_command_;
public:
	Controller(const std::vector<int>& vector) {
		model_ = new Model;
		view_ = new View;
		ModelBuilder obj(model_, vector, this);
		cur_command_ = nullptr;
	}
	void cout17El() override {
		cur_command_ = new Cout17El;
		done_commands_.push_back(cur_command_);
		cur_command_->setModel(model_);
		cur_command_->notify();
	}
	void activateView() const override {
		view_->activateView();
	}
	void sendElToView(int El) const override {
		view_->coutEl(El);
	}
};

class ProxyController : public IController {
private:
	IController*controller_;
public:
	ProxyController(const std::vector<int>& vector) {
		controller_ = new Controller(vector);
	}
	void cout17El() override {
		controller_->cout17El();
	}
	void activateView() const override {
		controller_->activateView();
	}
	void sendElToView(int El) const override {
		controller_->sendElToView(El);
	}
};