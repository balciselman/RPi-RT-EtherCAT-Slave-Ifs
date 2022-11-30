###############################################
#											  #
#											  #
# Makefile to build Etherslave interface and  #
# all libraries				  				  #
###############################################



jobs = $(shell nproc)
LIBDIR= ./lib


all:	create_libdir build_rpi_base build_rt_utilities build_shield_9252 build_thread build_gpioirq_kdriver build_slave_test LDCONFIG

debug:	create_libdir builddeb_rpi_base builddeb_rt_utilities builddeb_shield_9252 builddeb_thread build_gpioirq_kdriver builddeb_slave_test LDCONFIG

clean:	clean_rpi_base clean_rt_utilities clean_shield_9252 clean_thread clean_gpioirq_kdriver clean_slave_test clean_libdir

cleandbg:	cleandbg_rpi_base cleandbg_rt_utilities cleandbg_shield_9252 cleandbg_thread clean_gpioirq_kdriver cleandbg_slave_test clean_libdir

cleanall: clean cleandbg

rebuild: cleanall all

	
clean_libdir:
	rm -rf $(LIBDIR)

clean_rpi_base:		
	@( echo "###################################" ; echo "# cleaning rpi_base ... _________#"; echo "###################################" )
	make -C ./rpi_base clean
	@( echo "###################################" ; echo "# cleaning rpi_base done ________#"; echo "###################################" )



clean_rt_utilities:		
	@( echo "###################################" ; echo "# cleaning rt_utilities ... _________#"; echo "###################################" )
	make -C ./rt_utilities clean
	@( echo "###################################" ; echo "# cleaning rt_utilities done ________#"; echo "###################################" )


clean_shield_9252:		
	@( echo "###################################" ; echo "# cleaning shield_9252 ... _________#"; echo "###################################" )
	make -C ./shield_9252 clean
	@( echo "###################################" ; echo "# cleaning shield_9252 done ________#"; echo "###################################" )



clean_thread:		
	@( echo "###################################" ; echo "# cleaning thread ... _________#"; echo "###################################" )
	make -C ./thread clean
	@( echo "###################################" ; echo "# cleaning thread done ________#"; echo "###################################" )


clean_gpioirq_kdriver:		
	@( echo "###################################" ; echo "# cleaning gpioirq_kdriver ... _________#"; echo "###################################" )
	make -C ./gpioirq_kdriver clean
	@( echo "###################################" ; echo "# cleaning gpioirq_kdriver done ________#"; echo "###################################" )



clean_slave_test:		
	@( echo "###################################" ; echo "# cleaning slave_test ... _________#"; echo "###################################" )
	make -C ./slave_test clean
	@( echo "###################################" ; echo "# cleaning slave_test done ________#"; echo "###################################" )

#############################################################################################################

cleandbg_rpi_base:		
	@( echo "###################################" ; echo "# cleaning rpi_base_dbg ... _________#"; echo "###################################" )
	make -C ./rpi_base cleandbg
	@( echo "###################################" ; echo "# cleaning rpi_base_dbg done ________#"; echo "###################################" )



cleandbg_rt_utilities:		
	@( echo "###################################" ; echo "# cleaning rt_utilities_dbg ... _________#"; echo "###################################" )
	make -C ./rt_utilities cleandbg
	@( echo "###################################" ; echo "# cleaning rt_utilities_dbg done ________#"; echo "###################################" )


cleandbg_shield_9252:		
	@( echo "###################################" ; echo "# cleaning shield_9252_dbg ... _________#"; echo "###################################" )
	make -C ./shield_9252 cleandbg
	@( echo "###################################" ; echo "# cleaning shield_9252_dbg done ________#"; echo "###################################" )



cleandbg_thread:		
	@( echo "###################################" ; echo "# cleaning thread_dbg ... _________#"; echo "###################################" )
	make -C ./thread cleandbg
	@( echo "###################################" ; echo "# cleaning thread_dbg done ________#"; echo "###################################" )



cleandbg_slave_test:		
	@( echo "###################################" ; echo "# cleaning slave_test_dbg ... _________#"; echo "###################################" )
	make -C ./slave_test cleandbg
	@( echo "###################################" ; echo "# cleaning slave_test_dbg done ________#"; echo "###################################" )


###############################################################################################################
###############################################################################################################
###############################################################################################################


create_libdir:
	mkdir -p $(LIBDIR)

build_rpi_base:							
	@( echo "###################################" ; echo "# building rpi_base ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./rpi_base all
	@( echo "###################################" ; echo "# building rpi_base done ________#"; echo "###################################" )


build_rt_utilities:							
	@( echo "###################################" ; echo "# building rt_utilities ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./rt_utilities all
	@( echo "###################################" ; echo "# building rt_utilities done ________#"; echo "###################################" )


build_shield_9252:							
	@( echo "###################################" ; echo "# building shield_9252 ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./shield_9252 all
	@( echo "###################################" ; echo "# building shield_9252 done ________#"; echo "###################################" )


build_thread:							
	@( echo "###################################" ; echo "# building thread ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./thread all
	@( echo "###################################" ; echo "# building thread done ________#"; echo "###################################" )


build_gpioirq_kdriver:							
	@( echo "###################################" ; echo "# building gpioirq_kdriver ... _________#"; echo "###################################" )
	make  -C ./gpioirq_kdriver all
	@( echo "###################################" ; echo "# building gpioirq_kdriver done ________#"; echo "###################################" )


build_slave_test:							
	@( echo "###################################" ; echo "# building slave_test ... _________#"; echo "###################################" )
	make -C ./slave_test all
	@( echo "###################################" ; echo "# building slave_test done ________#"; echo "###################################" )


LDCONFIG:			
	@( echo "######### create ld.so.conf.d/ethercatslave.conf ##########################" )
	@( echo $(realpath $(LIBDIR)) > /etc/ld.so.conf.d/ethercatslave.conf )
	ldconfig


###########################################################################################################################

builddeb_rpi_base:							
	@( echo "###################################" ; echo "# building rpi_base_dbg ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./rpi_base debug
	@( echo "###################################" ; echo "# building rpi_base_dbg done ________#"; echo "###################################" )


builddeb_rt_utilities:							
	@( echo "###################################" ; echo "# building rt_utilities_dbg ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./rt_utilities debug
	@( echo "###################################" ; echo "# building rt_utilities_dbg done ________#"; echo "###################################" )


builddeb_shield_9252:							
	@( echo "###################################" ; echo "# building shield_9252_dbg ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./shield_9252 debug
	@( echo "###################################" ; echo "# building shield_9252_dbg done ________#"; echo "###################################" )


builddeb_thread:							
	@( echo "###################################" ; echo "# building thread_dbg ... _________#"; echo "###################################" )
	make -j $(jobs) -C ./thread debug
	@( echo "###################################" ; echo "# building thread_dbg done ________#"; echo "###################################" )



builddeb_slave_test:							
	@( echo "###################################" ; echo "# building slave_test_dbg ... _________#"; echo "###################################" )
	make -C ./slave_test debug
	@( echo "###################################" ; echo "# building slave_test_dbg done ________#"; echo "###################################" )

