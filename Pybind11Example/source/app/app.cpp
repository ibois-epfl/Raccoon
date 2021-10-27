//#include <iostream>
//#include <Python.h>
//#include <pybind11/embed.h>
//
//#include <stdio.h>
//
//void say_smth() {
//	printf("Say Something\n");
//}
//
//static int COUNTER = 0;
//void set_counter(int count) {
//	COUNTER = count;
//}
//
//PYBIND11_EMBEDDED_MODULE(embeddedmodule, module) {
//	module.doc() = "Embedded module";
//	module.def("say_smth", &say_smth);
//	module.def("set_counter", &set_counter);
//}
//
//void main(){
//
//	pybind11::scoped_interpreter guard{}; //allow python interpreter
//
//	pybind11::exec("print('hello world')");
//	pybind11::exec("import embeddedmodule");
//	pybind11::exec("embeddedmodule.say_smth()");
//	pybind11::exec("embeddedmodule.set_counter(10)");
//
//
//	auto sys = pybind11::module::import("sys");
//	pybind11::print(sys.attr("path"));
//	
//
//}