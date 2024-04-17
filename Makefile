
.PHONY: all clean

M1 := ./local_tcp_server
M2 := ./local_tcp_client

M3 := ./local_udp_server
M4 := ./local_udp_client

M5 := ./inet_tcp_server
M6 := ./inet_tcp_client

M7 := ./inet_udp_server
M8 := ./inet_udp_client

all:
	$(MAKE) -C $(M1) all
	$(MAKE) -C $(M2) all
	$(MAKE) -C $(M3) all
	$(MAKE) -C $(M4) all
	$(MAKE) -C $(M5) all
	$(MAKE) -C $(M6) all
	$(MAKE) -C $(M7) all
	$(MAKE) -C $(M8) all

clean:
	$(MAKE) -C $(M1) clean
	$(MAKE) -C $(M2) clean
	$(MAKE) -C $(M3) clean
	$(MAKE) -C $(M4) clean
	$(MAKE) -C $(M5) clean
	$(MAKE) -C $(M6) clean
	$(MAKE) -C $(M7) clean
	$(MAKE) -C $(M8) clean
