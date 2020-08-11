binary/coffee_machine: \
	src/coffee_machine/main.o 
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)
