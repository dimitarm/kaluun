# kaluun

Kaluun is (super) fast template rendering engine written in C++. The main motivation behind the effort for writting it was that it is really super fast (the fastest I've seen so far). It does not do any single unnecessary memory read from the source template text and tries to minimize all other operations like string copy. Despite of it being very fast it is highly componentized and canbe extended in many ways very easy. It was written with the paradigm in mind "compile (your template) once render many times" with different context values. 
