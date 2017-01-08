all:
	+$(MAKE) -C client
	+$(MAKE) -C probe
	+$(MAKE) -C server
clean:
	+$(MAKE) clean -C client
	+$(MAKE) clean -C probe
	+$(MAKE) clean -C server
