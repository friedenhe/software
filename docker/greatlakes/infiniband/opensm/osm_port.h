/*
 * Copyright (c) 2004-2009 Voltaire, Inc. All rights reserved.
 * Copyright (c) 2002-2011 Mellanox Technologies LTD. All rights reserved.
 * Copyright (c) 1996-2003 Intel Corporation. All rights reserved.
 * Copyright (C) 2012-2017 Tokyo Institute of Technology. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

/*
 * Abstract:
 * 	Declaration of port related objects.
 *	These objects comprise an IBA port.
 *	These objects are part of the OpenSM family of objects.
 */

#ifndef _OSM_PORT_H_
#define _OSM_PORT_H_

#include <sys/time.h>
#include <complib/cl_qmap.h>
#include <iba/ib_types.h>
#include <opensm/osm_base.h>
#include <opensm/osm_subnet.h>
#include <opensm/osm_madw.h>
#include <opensm/osm_path.h>
#include <opensm/osm_pkey.h>
#include <opensm/osm_topology_mgr.h>
#include <opensm/osm_vport.h>

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else				/* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif				/* __cplusplus */

#define AGUID_MAX_DESC (IB_NODE_DESCRIPTION_SIZE + 100)
#define OSM_PORT_TIMESTAMP_STR_LEN	32

BEGIN_C_DECLS
/*
	Forward references.
*/
struct osm_port;
struct osm_node;
struct osm_mgrp;
struct osm_sm;
struct osm_vport;


/****s* OpenSM: Virtualization Data/osm_virt_data_t
* NAME
*	osm_virt_data_t
*
* DESCRIPTION
*	This object represents all the virtualization related data which
*	is relevant for a osm_port_t object.
*
* SYNOPSIS
*/
typedef struct osm_virt_data {
	ib_virtualization_info_t virtualization_info;
	struct osm_vport **vports;
	uint16_t allocated_blocks;
	cl_list_item_t virt_mgr_queue_item;
	boolean_t is_virt_mgr_queue_pending;
	struct osm_port *p_port;
	uint16_t transactions;
	boolean_t first_virt_discovery;
	boolean_t state_change_bit_check;
} osm_virt_data_t;
/*
* FIELDS
*	virtualization_info
*		Virtualization info mad data
*
*	vports
*		Pointer to array of pointers to the vport objects which
*		belong to a certain port
*
*	allocated_blocks
*		Number of vports blocks which were allocated for the vports
*		array, each block size is OSM_VPORTS_BLOCK_SIZE
*
*	virt_mgr_queue_item
*		Linkage structure for cl_qlist. Used by Virtualization
*		Manger to queue ports that need to be processed.
*
*	is_virt_mgr_queue_pending
*		Flag indicating that the port is waiting to be processed by
*		Virtualization Manager.
*
*	transactions
*		Counter of the open (mad) transactions related to the
*		virtualization of the port, this counter is used to track
*		down timeouts in virtualization mads (vi/vps/vpi mads)
*
*	first_virt_discovery
*		Flag indicating that this is the first time we discover
*		virtualization data for this port (should be turned on
*		when discovering new port, discovering port in INIT state or
*		on first_master_sweep
*
*	state_change_bit_check
*		Flag indicating that OpenSM should query VPortStateChange of
*		this port, and perform vports discovery only if it's value
*		is 1.
*
* SEE ALSO
*
*********/


/****h* OpenSM/Physical Port
* NAME
*	Physical Port
*
* DESCRIPTION
*	The Physical Port object encapsulates the information needed by the
*	OpenSM to manage physical ports.  The OpenSM allocates one Physical Port
*	per physical port in the IBA subnet.
*
*	In a switch, one multiple Physical Port objects share the same port GUID.
*	In an end-point, Physical Ports do not share GUID values.
*
*	The Physical Port is not thread safe, thus callers must provide
*	serialization.
*
*	These objects should be treated as opaque and should be
*	manipulated only through the provided functions.
*
* AUTHOR
*	Steve King, Intel
*
*********/

/****s* OpenSM: Physical Port/osm_physp_t
* NAME
*	osm_physp_t
*
* DESCRIPTION
*	This object represents a physical port on a switch, router or end-point.
*
*	The osm_physp_t object should be treated as opaque and should
*	be manipulated only through the provided functions.
*
* SYNOPSIS
*/
typedef struct osm_physp {
	ib_port_info_t port_info;
	ib_mlnx_ext_port_info_t ext_port_info;
	ib_net64_t port_guid;
	ib_net64_t (*p_guids)[];
	uint8_t port_num;
	struct osm_node *p_node;
	struct osm_physp *p_remote_physp;
	struct osm_physp *p_prev_remote_physp;
	boolean_t healthy;
	unsigned num_dr_paths_tried;
	uint8_t vl_high_limit;
	unsigned need_update;
	unsigned is_prof_ignored;
	osm_dr_path_t dr_path;
	osm_pkey_tbl_t pkeys;
	ib_vl_arb_table_t vl_arb[4];
	cl_ptr_vector_t slvl_by_port;
	qos_config_sl_t qos_config_sl;
	uint8_t hop_wf;
	uint32_t mcast_route_weight;
	union {
		struct {
			ib_sw_cong_setting_t sw_cong_setting;
		} sw;
		struct {
			ib_ca_cong_setting_t ca_cong_setting;
			ib_cc_tbl_t cc_tbl[OSM_CCT_ENTRY_MAD_BLOCKS];
		} ca;
	} cc;
	uint64_t topo_id;
	boolean_t port_info_dirty;
	boolean_t ext_port_info_dirty;
	boolean_t ext_port_info_updated;
	osm_port_reset_type_enum port_reset_required;
	char state_timestamp_str[OSM_PORT_TIMESTAMP_STR_LEN];
} osm_physp_t;
/*
* FIELDS
*	port_info
*		The IBA defined PortInfo data for this port.
*
*	ext_port_info
*		Mellanox vendor specific ExtendedPortInfo data for this port.
*
*	port_guid
*		Port GUID value of this port.  For switches,
*		all ports share the same GUID value.
*
*	p_guids
*		Pointer to array of GUIDs obtained from GUIDInfo.
*		This pointer is null for switch physical/external ports
*		(used for endports only).
*
*	port_num
*		The port number of this port.  The PortInfo also
*		contains a port_number, but that number is not
*		the port number of this port, but rather the number
*		of the port that received the SMP during discovery.
*		Therefore, we must keep a separate record for this
*		port's port number.
*
*	p_node
*		Pointer to the parent Node object of this Physical Port.
*
*	p_remote_physp
*		Pointer to the Physical Port on the other side of the wire.
*		If this pointer is NULL no link exists at this port.
*
*	p_prev_remote_physp
*		Pointer to the Physical Port on the other side that was
*		connected to this port before the unlink.
*
*	healthy
*		Tracks the health of the port. Normally should be TRUE but
*		might change as a result of incoming traps indicating the port
*		healthy is questionable.
*
*	num_dr_paths_tried
*		The number of alternate DR paths that were tried after a transaction
*		timeout or error have occurred while accessing this physp
*
*	vl_high_limit
*		PortInfo:VLHighLimit value which installed by QoS manager
*		and should be uploaded to port's PortInfo
*
*	need_update
*		When set indicates that port was probably reset and port
*		related tables (PKey, SL2VL, VLArb) require refreshing.
*
*	is_prof_ignored
*		When set indicates that switch port will be ignored by
*		the link load equalization algorithm.
*
*	dr_path
*		The directed route path to this port.
*
*	pkeys
*		osm_pkey_tbl_t object holding the port PKeys.
*
*	vl_arb[]
*		Each Physical Port has 4 sections of VL Arbitration table.
*
*	slvl_by_port
*		A vector of pointers to the sl2vl tables (ordered by input port).
*		Switches have an entry for every other input port (inc SMA=0).
*		On CAs only one per port.
*
*	qos_config_sl
*		qos_config_sl_t object holding the port QosConfigSL values.
*
*	hop_wf
*		Hop weighting factor to be used in the routing.
*
*	mcast_route_weight
*		Mcast routing weight, number of mcast routings through rhis port
*
*	sw_cong_setting
*		Physical port switch congestion settings (switches only)
*
*	ca_cong_setting
*		Physical port ca congestion settings (cas only)
*
*	cc_tbl
*		Physical port ca congestion control table (cas only)
*
*	topo_id
*		ID of topology which added this physp to its subnet.
*		Filled only in main subnet physp (has no meaning in topology
*		subnet physp)
*
*	port_info_dirty
*		Indicator for wheter port_info of this port was changed, and
*		OpenSM needs to send PortInfo set with local copy.
*
*	ext_port_info_dirty
*		Indicator for wheter ext_port_info of this port was changed, and
*		OpenSM needs to send ExtPortInfo set with local copy.
*
*	ext_port_info_updated
*		Indicator, indicates that the port has updated copy of
*		ext_port_info.
*
*	port_reset_required
*		Indicator for whether this port needs to be reset.
*
*	state_timestamp_str
*		Timestamp of last time the port state was updated.
*
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Physical Port/osm_physp_construct
* NAME
*	osm_physp_construct
*
* DESCRIPTION
*	Constructs a Physical Port.
*
* SYNOPSIS
*/
void osm_physp_construct(IN osm_physp_t * p_physp);
/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object to initialize.
*
* RETURN VALUES
*	This function does not return a value.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_init
* NAME
*	osm_physp_init
*
* DESCRIPTION
*	Initializes a Physical Port for use.
*
* SYNOPSIS
*/
void osm_physp_init(IN osm_physp_t * p_physp, IN ib_net64_t port_guid,
		    IN uint8_t port_num, IN const struct osm_node *p_node,
		    IN osm_bind_handle_t h_bind, IN uint8_t hop_count,
		    IN const uint8_t * p_initial_path);
/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object to initialize.
*
*	port_guid
*		[in] GUID value of this port.  Switch ports all share
*		the same value.
*		Caller should use 0 if the guid is unknown.
*
*	port_num
*		[in] The port number of this port.
*
*	p_node
*		[in] Pointer to the parent Node object of this Physical Port.
*
*	h_bind
*		[in] Bind handle on which this port is accessed.
*		Caller should use OSM_BIND_INVALID_HANDLE if the bind
*		handle to this port is unknown.
*
*	hop_count
*		[in] Directed route hop count to reach this port.
*		Caller should use 0 if the hop count is unknown.
*
*	p_initial_path
*		[in] Pointer to the directed route path to reach this node.
*		Caller should use NULL if the path is unknown.
*
* RETURN VALUES
*	This function does not return a value.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Port/void osm_physp_destroy
* NAME
*	osm_physp_destroy
*
* DESCRIPTION
*	This function destroys a Port object.
*
* SYNOPSIS
*/
void osm_physp_destroy(IN osm_physp_t * p_physp);
/*
* PARAMETERS
*	p_port
*		[in] Pointer to a PhysPort object to destroy.
*
* RETURN VALUE
*	This function does not return a value.
*
* NOTES
*	Performs any necessary cleanup of the specified PhysPort object.
*	Further operations should not be attempted on the destroyed object.
*	This function should only be called after a call to osm_physp_construct or
*	osm_physp_init.
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Physical Port/osm_physp_is_valid
* NAME
*	osm_physp_is_valid
*
* DESCRIPTION
*	Returns TRUE if the Physical Port has been successfully initialized.
*	FALSE otherwise. Can be used only in case p_physp is guranteed
*	to be not NULL.
*
* SYNOPSIS
*/
static inline boolean_t osm_physp_is_valid(IN const osm_physp_t * p_physp)
{
	return (p_physp->port_guid != 0);
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns TRUE if the Physical Port has been successfully initialized.
*	FALSE otherwise.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/


/****f* OpenSM: Physical Port/osm_physp_get_num_dr_tried
* NAME
*	osm_physp_get_num_dr_tried
*
* DESCRIPTION
*	Returns the number of directed paths tried while accessing this physp
*
* SYNOPSIS
*/
static inline unsigned osm_physp_get_num_dr_tried(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	return p_physp->num_dr_paths_tried;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the number of directed paths tried while accessing this physp
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_set_num_dr_tried
* NAME
*	osm_physp_set_num_dr_tried
*
* DESCRIPTION
*	Returns the number of directed paths tried while accessing this physp
*
* SYNOPSIS
*/
static inline void osm_physp_set_num_dr_tried(IN osm_physp_t * p_physp,
					      IN unsigned tries)
{
	OSM_ASSERT(p_physp);
	p_physp->num_dr_paths_tried = tries;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	tries
*		[in] The new number of trials to store
*
* RETURN VALUES
*	NONE
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_is_healthy
* NAME
*	osm_physp_is_healthy
*
* DESCRIPTION
*	Returns TRUE if the Physical Port has been maked as healthy
*	FALSE otherwise. Can be used only in case p_physp is guranteed
*	to be not NULL.
*
* SYNOPSIS
*/
static inline boolean_t osm_physp_is_healthy(IN const osm_physp_t * p_physp)
{
	return p_physp->healthy;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns TRUE if the Physical Port has been maked as healthy
*	FALSE otherwise.
*  All physical ports are initialized as "healthy" but may be marked
*  otherwise if a received trap claims otherwise.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_link_is_healthy
* NAME
*	osm_link_is_healthy
*
* DESCRIPTION
*	Returns TRUE if the link given by the physical port is health,
*  and FALSE otherwise. Link is healthy if both its physical ports are
*  healthy
*
* SYNOPSIS
*/
boolean_t osm_link_is_healthy(IN const osm_physp_t * p_physp);
/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	TRUE if both physical ports on the link are healthy, and FALSE otherwise.
*  All physical ports are initialized as "healthy" but may be marked
*  otherwise if a received trap claiming otherwise.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_set_health
* NAME
*	osm_physp_set_health
*
* DESCRIPTION
*	Sets the port health flag. TRUE means the port is healthy and
*  should be used for packet routing. FALSE means it should be avoided.
*
* SYNOPSIS
*/
static inline void osm_physp_set_health(IN osm_physp_t * p_physp,
					IN boolean_t is_healthy)
{
	OSM_ASSERT(p_physp);
	p_physp->healthy = is_healthy;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	is_healthy
*		[in] The health value to be assigned to the port.
*		     TRUE if the Physical Port should been maked as healthy
*		     FALSE otherwise.
*
* RETURN VALUES
*  NONE
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_link_is_throttled
* NAME
*	osm_link_is_throttled
*
* DESCRIPTION
*	Returns TRUE if the link speed/width given by the physical port is
*	below the theoretical maximum enabled by both ports, and FALSE
*	otherwise.
*
* SYNOPSIS
*/
boolean_t osm_link_is_throttled(IN osm_physp_t * p_physp,
				IN const boolean_t subn_has_fdr10_enabled);
/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	subn_has_fdr10_enabled
*		[in] TRUE if FDR10 is enabled for subnet, or FALSE otherwise.
*
* RETURN VALUES
*	FALSE if both directions of the link have active link speed/width equal
*	to the common maximum which both sides have enabled, and TRUE otherwise.
*	The return value is independent of whether or not the link was
*	throttled intentionally by the admin or throttled automatically by the
*	link auto-negotiation.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_set_port_info
* NAME
*	osm_physp_set_port_info
*
* DESCRIPTION
*	Copies the PortInfo attribute into the Physical Port object
*	based on the PortState.
*
* SYNOPSIS
*/
void osm_physp_set_port_info(IN osm_physp_t * p_physp,
			     IN const ib_port_info_t * p_pi,
			     IN const struct osm_sm * p_sm,
			     IN boolean_t set_timestamp);

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	p_pi
*		[in] Pointer to the IBA defined PortInfo at this port number.
*
*	p_sm
*		[in] Pointer to an osm_sm_t object.
*
* 	set_timestamp
* 		[in] Force updating timestamp of Physical Port.
*
* RETURN VALUES
*	This function does not return a value.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_set_base_lid
* NAME
*	osm_physp_set_base_lid
*
* DESCRIPTION
*	Sets the base lid for this Physical Port.
*
* SYNOPSIS
*/
static inline void osm_physp_set_base_lid(IN osm_physp_t * p_physp,
					  IN ib_net16_t base_lid)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	p_physp->port_info.base_lid = base_lid;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	base_lid
*		[in] Lid to set.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_set_pkey_tbl
* NAME
*  osm_physp_set_pkey_tbl
*
* DESCRIPTION
*  Copies the P_Key table into the Physical Port object.
*
* SYNOPSIS
*/
void osm_physp_set_pkey_tbl(IN osm_log_t * p_log, IN const osm_subn_t * p_subn,
			    IN osm_physp_t * p_physp,
			    IN ib_pkey_table_t * p_pkey_tbl,
			    IN uint16_t block_num,
			    IN boolean_t is_set);
/*
* PARAMETERS
*	p_log
*		[in] Pointer to a log object.
*
*	p_subn
*		[in] Pointer to the subnet data structure.
*
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	p_pkey_tbl
*		[in] Pointer to the IBA defined P_Key table for this port
*		     number.
*
*	block_num
*		[in] The part of the P_Key table as defined in the IBA
*		     (valid values 0-2047, and is further limited by the
*		     partitionCap).
*
* RETURN VALUES
*  This function does not return a value.
*
* NOTES
*
* SEE ALSO
*  Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_pkey_tbl
* NAME
*  osm_physp_get_pkey_tbl
*
* DESCRIPTION
*  Returns a pointer to the P_Key table object of the Physical Port object.
*
* SYNOPSIS
*/
static inline const osm_pkey_tbl_t *osm_physp_get_pkey_tbl(IN const osm_physp_t
							   * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	/*
	   (14.2.5.7) - the block number valid values are 0-2047, and are
	   further limited by the size of the P_Key table specified by the
	   PartitionCap on the node.
	 */
	return &p_physp->pkeys;
};

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*  The pointer to the P_Key table object.
*
* NOTES
*
* SEE ALSO
*  Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_set_slvl_tbl
* NAME
*	osm_physp_set_slvl_tbl
*
* DESCRIPTION
*	Copies the SLtoVL attribute into the Physical Port object.
*
* SYNOPSIS
*/
static inline void osm_physp_set_slvl_tbl(IN osm_physp_t * p_physp,
					  IN ib_slvl_table_t * p_slvl_tbl,
					  IN uint8_t in_port_num)
{
	ib_slvl_table_t *p_tbl;

	OSM_ASSERT(p_slvl_tbl);
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	p_tbl = (ib_slvl_table_t *)cl_ptr_vector_get(&p_physp->slvl_by_port, in_port_num);
	*p_tbl = *p_slvl_tbl;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	p_slvl_tbl
*		[in] Pointer to the IBA defined SLtoVL map table for this
*		     port number.
*
*	in_port_num
*		[in] Input Port Number for this SLtoVL.
*
* RETURN VALUES
*	This function does not return a value.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_slvl_tbl
* NAME
*	osm_physp_get_slvl_tbl
*
* DESCRIPTION
*	Returns a pointer to the SLtoVL attribute of the Physical Port object.
*
* SYNOPSIS
*/
static inline ib_slvl_table_t *osm_physp_get_slvl_tbl(IN const osm_physp_t *
						      p_physp,
						      IN uint8_t in_port_num)
{
	ib_slvl_table_t *p_tbl;

	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	p_tbl = (ib_slvl_table_t *)cl_ptr_vector_get(&p_physp->slvl_by_port, in_port_num);
	return p_tbl;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	in_port_num
*		[in] Input Port Number for this SLtoVL.
*
* RETURN VALUES
*	The pointer to the slvl table
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_set_vla_tbl
* NAME
*	osm_physp_set_vla_tbl
*
* DESCRIPTION
*	Copies the VL Arbitration attribute into the Physical Port object.
*
* SYNOPSIS
*/
static inline void osm_physp_set_vla_tbl(IN osm_physp_t * p_physp,
					 IN ib_vl_arb_table_t * p_vla_tbl,
					 IN uint8_t block_num)
{
	OSM_ASSERT(p_vla_tbl);
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	OSM_ASSERT((1 <= block_num) && (block_num <= 4));
	p_physp->vl_arb[block_num - 1] = *p_vla_tbl;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	p_vla_tbl
*		[in] Pointer to the IBA defined VL Arbitration table for this
*		     port number.
*
*	block_num
*		[in] The part of the VL arbitration as defined in the IBA
*		     (valid values 1-4)
*
* RETURN VALUES
*	This function does not return a value.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_vla_tbl
* NAME
*	osm_physp_get_vla_tbl
*
* DESCRIPTION
*	Returns a pointer to the VL Arbitration table of the Physical Port object.
*
* SYNOPSIS
*/
static inline ib_vl_arb_table_t *osm_physp_get_vla_tbl(IN osm_physp_t * p_physp,
						       IN uint8_t block_num)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	OSM_ASSERT((1 <= block_num) && (block_num <= 4));
	return &(p_physp->vl_arb[block_num - 1]);
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	block_num
*		[in] The part of the VL arbitration as defined in the IBA
*		     (valid values 1-4)
*
* RETURN VALUES
*  The pointer to the VL Arbitration table
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_remote
* NAME
*	osm_physp_get_remote
*
* DESCRIPTION
*	Returns a pointer to the Physical Port on the other side the wire.
*
* SYNOPSIS
*/
static inline osm_physp_t *osm_physp_get_remote(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return p_physp->p_remote_physp;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns a pointer to the Physical Port on the other side of
*	the wire.  A return value of NULL means there is no link at this port.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_port_guid
* NAME
*	osm_physp_get_port_guid
*
* DESCRIPTION
*	Returns the port guid of this physical port.
*
* SYNOPSIS
*/
static inline ib_net64_t osm_physp_get_port_guid(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return p_physp->port_guid;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the port guid of this physical port.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_subnet_prefix
* NAME
*	osm_physp_get_subnet_prefix
*
* DESCRIPTION
*	Returns the subnet prefix for this physical port.
*
* SYNOPSIS
*/
static inline ib_net64_t
osm_physp_get_subnet_prefix(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return p_physp->port_info.subnet_prefix;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the subnet prefix for this physical port.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_link_exists
* NAME
*	osm_physp_link_exists
*
* DESCRIPTION
*	Returns TRUE if the Physical Port has a link to the specified port.
*	FALSE otherwise.
*
* SYNOPSIS
*/
static inline boolean_t osm_physp_link_exists(IN const osm_physp_t * p_physp,
					      IN const osm_physp_t * p_remote_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(p_remote_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	OSM_ASSERT(osm_physp_is_valid(p_remote_physp));
	return ((p_physp->p_remote_physp == p_remote_physp) &&
		(p_remote_physp->p_remote_physp == p_physp));
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	p_remote_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns TRUE if the Physical Port has a link to another port.
*	FALSE otherwise.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_link
* NAME
*	osm_physp_link
*
* DESCRIPTION
*	Sets the pointers to the Physical Ports on the other side the wire.
*
* SYNOPSIS
*/
static inline void osm_physp_link(IN osm_physp_t * p_physp,
				  IN osm_physp_t * p_remote_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(p_remote_physp);
	p_physp->p_remote_physp = p_remote_physp;
	p_remote_physp->p_remote_physp = p_physp;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object to link.
*
*	p_remote_physp
*		[in] Pointer to the adjacent osm_physp_t object to link.
*
* RETURN VALUES
*	None.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_unlink
* NAME
*	osm_physp_unlink
*
* DESCRIPTION
*	Clears the pointers to the Physical Port on the other side the wire.
*
* SYNOPSIS
*/
static inline void osm_physp_unlink(IN osm_physp_t * p_physp,
				    IN osm_physp_t * p_remote_physp)
{
	OSM_ASSERT(osm_physp_link_exists(p_physp, p_remote_physp));
	p_physp->p_remote_physp = NULL;
	p_remote_physp->p_remote_physp = NULL;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object to link.
*
*	p_remote_physp
*		[in] Pointer to the adjacent osm_physp_t object to link.
*
* RETURN VALUES
*	None.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_has_any_link
* NAME
*	osm_physp_has_any_link
*
* DESCRIPTION
*	Returns TRUE if the Physical Port has a link to another port.
*	FALSE otherwise.
*
* SYNOPSIS
*/
static inline boolean_t osm_physp_has_any_link(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	if (osm_physp_is_valid(p_physp))
		return (p_physp->p_remote_physp != NULL);
	else
		return FALSE;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns TRUE if the Physical Port has a link to another port.
*	FALSE otherwise.
*
* NOTES
*
* SEE ALSO
*	Port, Physical Port
*********/

/****f* OpenSM: Physical Port/osm_physp_get_port_num
* NAME
*	osm_physp_get_port_num
*
* DESCRIPTION
*	Returns the local port number of this Physical Port.
*
* SYNOPSIS
*/
static inline uint8_t osm_physp_get_port_num(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return p_physp->port_num;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the local port number of this Physical Port.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_get_node_ptr
* NAME
*	osm_physp_get_node_ptr
*
* DESCRIPTION
*	Returns a pointer to the parent Node object for this port.
*
* SYNOPSIS
*/
static inline struct osm_node *osm_physp_get_node_ptr(IN const osm_physp_t *
						       p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return p_physp->p_node;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns a pointer to the parent Node object for this port.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_get_port_state
* NAME
*	osm_physp_get_port_state
*
* DESCRIPTION
*	Returns the port state of this Physical Port.
*
* SYNOPSIS
*/
static inline uint8_t osm_physp_get_port_state(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return ib_port_info_get_port_state(&p_physp->port_info);
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the local port number of this Physical Port.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_get_base_lid
* NAME
*	osm_physp_get_base_lid
*
* DESCRIPTION
*	Returns the base lid of this Physical Port.
*
* SYNOPSIS
*/
static inline ib_net16_t osm_physp_get_base_lid(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return p_physp->port_info.base_lid;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the base lid of this Physical Port.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_get_lmc
* NAME
*	osm_physp_get_lmc
*
* DESCRIPTION
*	Returns the LMC value of this Physical Port.
*
* SYNOPSIS
*/
static inline uint8_t osm_physp_get_lmc(IN const osm_physp_t * p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return ib_port_info_get_lmc(&p_physp->port_info);
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
* RETURN VALUES
*	Returns the LMC value of this Physical Port.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Physical Port/osm_physp_get_dr_path_ptr
* NAME
*	osm_physp_get_dr_path_ptr
*
* DESCRIPTION
*	Returns a pointer to the directed route path for this port.
*
* SYNOPSIS
*/
static inline osm_dr_path_t *osm_physp_get_dr_path_ptr(IN const osm_physp_t *
							p_physp)
{
	OSM_ASSERT(p_physp);
	OSM_ASSERT(osm_physp_is_valid(p_physp));
	return (osm_dr_path_t *) & p_physp->dr_path;
}

/*
* PARAMETERS
*	p_physp
*		[in] Pointer to a Physical Port object.
*
* RETURN VALUES
*	Returns a pointer to the directed route path for this port.
*
* NOTES
*
* SEE ALSO
*	Physical Port object
*********/

/****h* OpenSM/Port
* NAME
*	Port
*
* DESCRIPTION
*	The Port object encapsulates the information needed by the
*	OpenSM to manage ports.  The OpenSM allocates one Port object
*	per port in the IBA subnet.
*
*	Each Port object is associated with a single port GUID.  A Port object
*	contains 1 or more Physical Port objects.  An end point node has
*	one Physical Port per Port.  A switch node has more than
*	one Physical Port per Port.
*
*	The Port object is not thread safe, thus callers must provide
*	serialization.
*
*	These objects should be treated as opaque and should be
*	manipulated only through the provided functions.
*
* AUTHOR
*	Steve King, Intel
*
*********/

/****s* OpenSM: Port/osm_port_t
* NAME
*	osm_port_t
*
* DESCRIPTION
*	This object represents a logical port on a switch, router, or CA.
*
*	The osm_port_t object should be treated as opaque and should
*	be manipulated only through the provided functions.
*
* SYNOPSIS
*/
typedef struct osm_port {
	cl_map_item_t map_item;
	cl_list_item_t list_item;
	struct osm_node *p_node;
	ib_net64_t guid;
	uint32_t discovery_count;
	unsigned is_new;
	disc_state_t disc_through;
	boolean_t client_rereg;
	osm_physp_t *p_physp;
	cl_qlist_t mcm_list;
	int flag;
	int use_scatter;
	unsigned int cc_timeout_count;
	int cc_unavailable_flag;
	void *priv;
	ib_net16_t lid;
	cl_list_t topology_list;
	osm_virt_data_t virt_data;
} osm_port_t;
/*
* FIELDS
*	map_item
*		Linkage structure for cl_qmap.  MUST BE FIRST MEMBER!
*
*	list_item
*		Linkage structure for cl_qlist. Used by ucast mgr during
*		LFT calculation.
*
*	p_node
*		Points to the Node object that owns this port.
*
*	guid
*		Manufacturer assigned GUID for this port.
*
*	discovery_count
*		The number of times this port has been discovered
*		during the current fabric sweep.  This number is reset
*		to zero at the start of a sweep.
*
*	is_new
*		Indicates a port that was discovered for the first
*		time.
*
*	disc_through
*		Flag that indicates whether this port was discovered
*		through a ignored port or not.
*
*	client_rereg
*		Indicates that client_rereg has to be resent because
*		PortInfo timed out or because the port was ignored earlier.
*
*	p_physp
*		The pointer to physical port used when physical
*		characteristics contained in the Physical Port are needed.
*
*	mcm_list
*		Multicast member list
*
*	flag
*		Utility flag for port management
*
*	cc_timeout_count
*		Count number of times congestion control config times out.
*
*	cc_unavailable_flag
*		Flag indicating if congestion control is not supported.
*
*	virt_data
*		virtualization related data object
*
* SEE ALSO
*	Port, Physical Port, Physical Port Table
*********/

/****f* OpenSM: Port/osm_port_delete
* NAME
*	osm_port_delete
*
* DESCRIPTION
*	This function destroys and deallocates a Port object.
*
* SYNOPSIS
*/
void osm_port_delete(IN struct osm_sm *p_sm, IN OUT osm_port_t ** pp_port);
/*
* PARAMETERS
*	p_sm
*		[in] pointer to sm object
*
*	pp_port
*		[in][out] Pointer to a pointer to a Port object to delete.
*		On return, this pointer is NULL.
*
* RETURN VALUE
*	This function does not return a value.
*
* NOTES
*	Performs any necessary cleanup of the specified Port object.
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_new
* NAME
*	osm_port_new
*
* DESCRIPTION
*	This function allocates and initializes a Port object.
*
* SYNOPSIS
*/
osm_port_t *osm_port_new(IN const ib_node_info_t * p_ni,
			 IN struct osm_node *p_parent_node);
/*
* PARAMETERS
*	p_ni
*		[in] Pointer to the NodeInfo attribute relevant for this port.
*
*	p_parent_node
*		[in] Pointer to the initialized parent osm_node_t object
*		that owns this port.
*
* RETURN VALUE
*	Pointer to the initialize Port object.
*
* NOTES
*	Allows calling other port methods.
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_get_base_lid
* NAME
*	osm_port_get_base_lid
*
* DESCRIPTION
*	Gets the base LID of a port.
*
* SYNOPSIS
*/
static inline ib_net16_t osm_port_get_base_lid(IN const osm_port_t * p_port)
{
	return osm_physp_get_base_lid(p_port->p_physp);
}

/*
* PARAMETERS
*	p_port
*		[in] Pointer to a Port object.
*
* RETURN VALUE
*	Base LID of the port.
*	If the return value is 0, then this port has no assigned LID.
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_clear_base_lid
* NAME
*	osm_port_clear_base_lid
*
* DESCRIPTION
*	Clear the base lid of a given port.
*
* SYNOPSIS
*/
static inline void osm_port_clear_base_lid(OUT osm_port_t * p_port)
{
	OSM_ASSERT(p_port);
	p_port->lid = 0;
	osm_physp_set_base_lid(p_port->p_physp, 0);
}

/*
* PARAMETERS
*	p_port
*		[out] Pointer to a Port object.
*
* RETURN VALUE
*	None
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_get_lmc
* NAME
*	osm_port_get_lmc
*
* DESCRIPTION
*	Gets the LMC value of a port.
*
* SYNOPSIS
*/
static inline uint8_t osm_port_get_lmc(IN const osm_port_t * p_port)
{
	return osm_physp_get_lmc(p_port->p_physp);
}

/*
* PARAMETERS
*	p_port
*		[in] Pointer to a Port object.
*
* RETURN VALUE
*	Gets the LMC value of a port.
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_get_guid
* NAME
*	osm_port_get_guid
*
* DESCRIPTION
*	Gets the GUID of a port.
*
* SYNOPSIS
*/
static inline ib_net64_t osm_port_get_guid(IN const osm_port_t * p_port)
{
	return p_port->guid;
}

/*
* PARAMETERS
*	p_port
*		[in] Pointer to a Port object.
*
* RETURN VALUE
*	Manufacturer assigned GUID of the port.
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_get_node
* NAME
*	osm_port_get_node
*
* DESCRIPTION
*	Gets the pointer to the port node
*
* SYNOPSIS
*/
static inline struct osm_node* osm_port_get_node(IN const osm_port_t * p_port)
{
	return p_port->p_node;
}

/*
* PARAMETERS
*	p_port
*		[in] Pointer to a Port object.
*
* RETURN VALUE
*	The pointer to the node containing the port
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_get_physp
* NAME
*	osm_port_get_physp
*
* DESCRIPTION
*	Gets the pointer to the port physp
*
* SYNOPSIS
*/
static inline osm_physp_t* osm_port_get_physp(IN const osm_port_t * p_port)
{
	return p_port->p_physp;
}

/*
* PARAMETERS
*	p_port
*		[in] Pointer to a Port object.
*
* RETURN VALUE
*	The pointer to the physp of the port
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_port_get_lid_range_ho
* NAME
*	osm_port_get_lid_range_ho
*
* DESCRIPTION
*	Returns the HOST ORDER lid min and max values for this port,
*	based on the lmc value.
*
* SYNOPSIS
*/
void osm_port_get_lid_range_ho(IN const osm_port_t * p_port,
			       OUT uint16_t * p_min_lid,
			       OUT uint16_t * p_max_lid);
/*
* PARAMETERS
*	p_port
*		[in] Pointer to a Port object.
*
*	p_min_lid
*		[out] Pointer to the minimum LID value occupied by this port.
*
*	p_max_lid
*		[out] Pointer to the maximum LID value occupied by this port.
*
* RETURN VALUE
*	None.
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Physical Port/osm_physp_calc_link_mtu
* NAME
*	osm_physp_calc_link_mtu
*
* DESCRIPTION
*	Calculate the Port MTU based on current and remote
*  physical ports MTU CAP values.
*
* SYNOPSIS
*/
uint8_t osm_physp_calc_link_mtu(IN osm_log_t * p_log,
				IN const osm_physp_t * p_physp,
				IN uint8_t current_mtu);
/*
* PARAMETERS
*	p_log
*		[in] Pointer to a log object.
*
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	current_mtu
*		[in] Current neighbor mtu on this port
*
* RETURN VALUES
*	The MTU of the link to be used.
*
* NOTES
*
* SEE ALSO
*	PhysPort object
*********/

/****f* OpenSM: Physical Port/osm_physp_calc_link_op_vls
* NAME
*	osm_physp_calc_link_op_vls
*
* DESCRIPTION
*	Calculate the Port OP_VLS based on current and remote
*  physical ports VL CAP values. Allowing user option for a max limit.
*
* SYNOPSIS
*/
uint8_t osm_physp_calc_link_op_vls(IN osm_log_t * p_log,
				   IN const osm_subn_t * p_subn,
				   IN const osm_physp_t * p_physp,
				   IN uint8_t current_op_vls);
/*
* PARAMETERS
*	p_log
*		[in] Pointer to a log object.
*
*	p_subn
*		[in] Pointer to the subnet object for accessing of the options.
*
*	p_physp
*		[in] Pointer to an osm_physp_t object.
*
*	current_op_vls
*		[in] Current operational VL on the port
*
* RETURN VALUES
*	The OP_VLS of the link to be used.
*
* NOTES
*
* SEE ALSO
*  PhysPort object
*********/

/****f* OpenSM: Physical Port/osm_physp_replace_dr_path
* NAME
*	osm_physp_replace_dr_path
*
* DESCRIPTION
*	Replace the direct route path for the given phys port with an
*  alternate path going through forien set of phys port.
*
* SYNOPSIS
*/
boolean_t
osm_physp_replace_dr_path(IN osm_log_t * p_log,
			  IN osm_subn_t const *p_subn,
			  IN osm_physp_t *p_dest_physp);
/*
* PARAMETERS
*	p_log
*		[in] Pointer to a log object.
*
*	p_subn
*		[in] Pointer to the subnet object for accessing of the options.
*
*	p_dest_physp
*		[in] Pointer to an osm_physp_t object which is the destination
*
* RETURN VALUES
*	TRUE if successful FALSE otherwise
*
* NOTES
*
* SEE ALSO
*	PhysPort object
*********/

/****s* OpenSM: Port/osm_alias_guid_t
* NAME
*	osm_alias_guid_t
*
* DESCRIPTION
*	This object represents an alias guid for an endport.
*
*	The osm_alias_guid_t object should be treated as opaque and should
*	be manipulated only through the provided functions.
*
* SYNOPSIS
*/
typedef struct osm_alias_guid {
	cl_map_item_t map_item;
	ib_net64_t alias_guid;
	ib_net64_t base_port_guid;
	osm_port_t *p_base_port;
	boolean_t dirty;
	uint16_t vport_index;
} osm_alias_guid_t;
/*
* FIELDS
*	map_item
*		Linkage structure for cl_qmap.  MUST BE FIRST MEMBER!
*
*	alias_guid
*		Alias GUID for port obtained from SM GUIDInfo attribute
*
*	p_base_port
*		Pointer to osm_port_t for base port GUID
*
*	p_vport_index
*		The index of the vport in the port in case that this is a
*		vport guid
*
* SEE ALSO
*	Port, Physical Port, Physical Port Table
*/

/****f* OpenSM: Port/osm_alias_guid_new
* NAME
*	osm_alias_guid_new
*
* DESCRIPTION
*	This function allocates and initializes an alias guid object.
*
* SYNOPSIS
*/
osm_alias_guid_t *osm_alias_guid_new(IN const ib_net64_t alias_guid,
				     IN osm_port_t *p_base_port,
				     IN uint16_t vport_index);
/*
* PARAMETERS
*	alias_guid
*		[in] Alias GUID in network order
*
*	p_base_port
*		[in] Pointer to the port for this base GUID
*
*	p_vport_index
*		The index of the vport in the port in case that this is a
*		vport guid
*
* RETURN VALUE
*	Pointer to the initialized alias guid object.
*
* NOTES
*	Allows calling other alias guid methods.
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_alias_guid_delete
* NAME
*	osm_alias_guid_delete
*
* DESCRIPTION
*	This function destroys and deallocates an alias guid object.
*
* SYNOPSIS
*/
void osm_alias_guid_delete(IN OUT osm_alias_guid_t ** pp_alias_guid);
/*
* PARAMETERS
*	pp_alias_guid
*		[in][out] Pointer to a pointer to an alias guid object to delete.
*		On return, this pointer is NULL.
*
* RETURN VALUE
*	This function does not return a value.
*
* NOTES
*	Performs any necessary cleanup of the specified alias guid object.
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_alias_guid_get_alias_guid
* NAME
*	osm_alias_guid_get_alias_guid
*
* DESCRIPTION
*	This function retrieves alias guid from alias guid object.
*
* SYNOPSIS
*/
static inline ib_net64_t osm_alias_guid_get_alias_guid(IN osm_alias_guid_t *p_alias_guid)
{
	OSM_ASSERT(p_alias_guid);
	return p_alias_guid->alias_guid;
}
/*
* PARAMETERS
*	p_alias_guid
*		[in] Pointer to a pointer to an alias guid object.
*
* RETURN VALUE
*	This function returns the alias guid or NULL if fails.
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_alias_guid_get_base_guid
* NAME
*	osm_alias_guid_get_base_guid
*
* DESCRIPTION
*	This function retrieves base guid from alias guid object.
*
* SYNOPSIS
*/
static inline ib_net64_t osm_alias_guid_get_base_guid(IN osm_alias_guid_t *p_alias_guid)
{
	OSM_ASSERT(p_alias_guid);
	return p_alias_guid->base_port_guid;
}
/*
* PARAMETERS
*	p_alias_guid
*		[in] Pointer to a pointer to an alias guid object.
*
* RETURN VALUE
*	This function returns the base guid or NULL if fails.
*
* NOTES
*
* SEE ALSO
*	Port
*********/

/****f* OpenSM: Port/osm_alias_guid_get_pkey_tbl
* NAME
*	osm_alias_guid_get_pkey_tbl
*
* DESCRIPTION
*	This function retrieves pkey table from alias guid object.
*
* SYNOPSIS
*/
const osm_pkey_tbl_t *osm_alias_guid_get_pkey_tbl(IN const osm_alias_guid_t *p_alias_guid);
/*
* PARAMETERS
*	p_alias_guid
*		[in] Pointer to a pointer to an alias guid object.
*
* RETURN VALUE
*	This function returns the base guid or NULL if fails.
*
* NOTES
*
* SEE ALSO
*	Port
*********/


/****f* OpenSM: Port/osm_alias_guid_desc
* NAME
*	osm_alias_guid_desc
*
* DESCRIPTION
*	This function builds a description string for alias guid.
*
* SYNOPSIS
*/
void osm_alias_guid_desc(IN const osm_alias_guid_t *p_alias_guid, OUT char *desc);
/*
* PARAMETERS
*	p_alias_guid
*		[in] Pointer to an alias guid object.
*	desc
*		[out] string to fill in with the created desc
*
* RETURN VALUE
*	This function does not return a value
*
* NOTES
*
* SEE ALSO
*	osm_alias_guid_t
*********/

/****f* OpenSM: Port/osm_port_is_isolated
* NAME
*	osm_port_is_isolated
*
* DESCRIPTION
*	Returns true if the port is isolated
*
* SYNOPSIS
*/
static inline boolean_t osm_port_is_isolated(IN const osm_port_t * p_port)
{
	return p_port->disc_through == OSM_DISC_IGNORED;
}
/*
* PARAMETERS
*	p_port
*		[in] Pointer to the port.
*
* RETURN VALUE
*	Returns true if the port is isolated.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_set_virt_info
* NAME
*	osm_port_set_virt_info
*
* DESCRIPTION
*	This method set virtualization info mad data in the virt_data of the
*	port and in addition it adjust vports array size to the new
*	top index value in the mad.
*	After this method is called it is safe to access vports up to the
*	top index value received in the virtualization info mad
* SYNOPSIS
*/
boolean_t osm_port_set_virt_info(IN struct osm_sm *p_sm, osm_port_t * p_port,
				 IN const ib_virtualization_info_t *p_vi);
/*
* PARAMETERS
*	p_sm
*		[in] Pointer to sm object
*	p_port
*		Pointer to the port.
*
*	p_vi
*		[in] Pointer to virtualization info object.
*
* RETURN VALUE
*	Boolean indication whether setting succeeded.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_get_vport_top_index
* NAME
*	osm_port_get_vport_top_index
*
* DESCRIPTION
*	Get the top index (the max vport index)
*
* SYNOPSIS
*/
static inline uint16_t osm_port_get_vport_top_index(IN const osm_port_t * p_port)
{
	return cl_ntoh16(p_port->virt_data.virtualization_info.vport_index_top);
}
/*
* PARAMETERS
*	p_port
*		[in] Pointer to the port.
*
* RETURN VALUE
*	Return the max vport index (0 if no virtualization info)
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_get_vport
* NAME
*	osm_port_get_vport
*
* DESCRIPTION
*	Get the vport in a specific index
*
* SYNOPSIS
*/
static inline struct osm_vport *osm_port_get_vport(IN const osm_port_t * p_port,
						   IN uint16_t index)
{
	if (!p_port->virt_data.vports ||
	    (index > osm_port_get_vport_top_index(p_port)))
		return NULL;

	return p_port->virt_data.vports[index];
}
/*
* PARAMETERS
*	p_port
*		[in] Pointer to the port.
*
*	index
*		[in] Index of vport to get
*
* RETURN VALUE
*	Pointer to required vport or NULL if no vport in this index.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_create_vport
* NAME
*	osm_port_create_vport
*
* DESCRIPTION
*	Create vport in a port
*
* SYNOPSIS
*/
osm_vport_t *osm_port_create_vport(IN struct osm_sm *p_sm, osm_port_t *p_port,
				   IN uint16_t index, IN ib_vport_info_t *p_vpi,
				   IN osm_vpi_context_t *p_vpi_context);
/*
* PARAMETERS
*	p_sm
*		[in] Pointer to sm object
*
*	p_port
*		Pointer to the port which this vport will be created in.
*
*	index
*		[in] Index of vport to create
*
*	p_vpi
*		[in] pointer to osm_vport_info_t object of vport to create
*
*	p_vpi_context
*		[in] Pointer to VPortInfo context object
*
* RETURN VALUE
*	Pointer to created vport (if creation succeeded)
*	NULL in case of failure.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_delete_vport
* NAME
*	osm_port_delete_vport
*
* DESCRIPTION
*	Delete vport in a specific index
*
* SYNOPSIS
*/
void osm_port_delete_vport(IN struct osm_sm *p_sm, osm_port_t *p_port,
			   IN uint16_t index);
/*
* PARAMETERS
*	p_sm
*		[in] Pointer to sm object.
*
*	p_port
*		[in] Pointer to the port.
*
*	index
*		[in] Index of vport to get
*
* RETURN VALUE
*	No value returned
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_update_vport
* NAME
*	osm_port_update_vport
*
* DESCRIPTION
*	Update vport in a specific index
*
* SYNOPSIS
*/
void osm_port_update_vport(IN struct osm_sm *p_sm, osm_port_t *p_port,
			   IN uint16_t index, IN ib_vport_info_t *p_vpi,
			   IN osm_vpi_context_t *p_vpi_context);
/*
* PARAMETERS
*	p_sm
*		Pointer to sm object.
*
*	p_port
*		Pointer to the port.
*
*	index
*		[in] Index of vport to get
*
*	p_vpi
*		[in] Pointer to updated VportInfo object
*
*	p_vpi_context
*		[in] Pointer to VPortInfo context object
*
* RETURN VALUE
*	No value returned
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_decrease_virt_transactions
* NAME
*	osm_port_decrease_virt_transactions
*
* DESCRIPTION
*	Decrease the transaction counter of the virtualization data of the port
*
* SYNOPSIS
*/
void osm_port_decrease_virt_transactions(IN struct osm_sm *p_sm,
					 osm_port_t *p_port);
/*
* PARAMETERS
*	p_sm
*		Pointer to sm object.
*
*	p_port
*		Pointer to the port.
*
* RETURN VALUE
*	No value returned
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_increase_virt_transactions
* NAME
*	osm_port_increase_virt_transactions
*
* DESCRIPTION
*	Increase the transaction counter of the virtualization data of the port
*
* SYNOPSIS
*/
void osm_port_increase_virt_transactions(IN struct osm_sm *p_sm,
					 osm_port_t *p_port);
/*
* PARAMETERS
*	p_sm
*		Pointer to sm object.
*
*	p_port
*		Pointer to the port.
*
* RETURN VALUE
*	No value returned
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_delete_virtualization
* NAME
*	osm_port_delete_virtualization
*
* DESCRIPTION
*	This function deletes all virtualization info of of the port.
*
* SYNOPSIS
*/

void osm_port_delete_virtualization(IN struct osm_sm *p_sm,
                                    IN OUT osm_port_t *p_port);
/*
* PARAMETERS
*	p_sm
*		[in] Pointer to sm object
*	p_port
*		Pointer to the port.
*
* RETURN VALUES
*	This function does not return a value.
*
* NOTES
*
* SEE ALSO
*********/

/****f* OpenSM: Port/osm_port_is_virt_supported
* NAME
*	osm_port_is_virt_supported
*
* DESCRIPTION
*	Check if the port support virtualization.
*
* SYNOPSIS
*/

boolean_t osm_port_is_virt_supported(IN osm_port_t *p_port);
/*
* PARAMETERS
*	p_port
*		Pointer to the port.
*
* RETURN VALUES
*	Returns true if the port supports virtualization, otherwise false.
*
* NOTES
*	The fanction can be used only after general info was received for this
*	port.
*
* SEE ALSO
*********/

/****f* OpenSM: Topologies mgr/osm_topo_physp_on_main
* NAME osm_topo_physp_on_main
*
* DESCRIPTION
*	Get iterator from the topology_list and return pointer to toplogy
*	refered by this iterator
*
* SYNOPSIS
*
*/
static inline osm_topology_t *osm_port_get_topo_from_list_iter(cl_list_iterator_t *iter)
{
	osm_topology_t **pp_topo = (osm_topology_t **)cl_list_obj(*iter);
	return *pp_topo;
}
/*
* PARAMETERS
*	iter
*		Pointer to iterator form the topology_list.
*
* RETURN VALUE
*	Return pointer to the topology refered by the iterator
*/

/****f* OpenSM: Port/osm_port_is_vport_lid_assigned
* NAME osm_port_is_vport_lid_assigned
*
* DESCRIPTION
*	Check if lid is assigned for a given vport index
*
* SYNOPSIS
*
*/
boolean_t osm_port_is_vport_lid_assigned(osm_port_t *p_port,
					 uint16_t vport_index);
/*
* PARAMETERS
*	p_port
*		Pointer to port struct
*
*	vport_index
*		vport index to check if lid is assigned to
*
* RETURN VALUE
*	Boolean indication whether lid is assigned for vport in given index
*/

/****f* OpenSM: Port/osm_port_get_vport_by_lid
* NAME osm_port_get_vport_by_lid
*
* DESCRIPTION
*	Return a VPort which is specifically assigned for a given lid.
*
*	This function will return a VPort only on below conditions:
*
*	1) This port runs VPorts which requires lids.
*	2) One of the VPorts (Non-VPort0) required a lid and assigned the lid
*		that is given as input to this function
*
* SYNOPSIS
*
*/
osm_vport_t *osm_port_get_vport_by_lid(const osm_port_t *p_port,
				       ib_net16_t lid);
/*
* PARAMETERS
*	p_port
*		Pointer to port struct
*
*	lid
*		The lid to search
*
* RETURN VALUE
*	Non-VPort0 Vport - In case that given lid assigned specifically for
*				this vport
*
*	NULL - If no Non-VPort0 VPort assigned for this lid
*/

/****f* OpenSM: Port/osm_port_update_vports_mcast_membership
* NAME osm_port_update_vports_mcast_membership
*
* DESCRIPTION
*	Remove vports of the given port from multicast group
*	if vports are not sharing the PKEY of this group
*
* SYNOPSIS
*
*/

void osm_port_update_vports_mcast_membership(IN struct osm_sm *sm,
					     IN osm_port_t *p_port,
					     IN struct osm_mgrp *mgrp);

/*
* PARAMETERS
*	sm
*		[in] Pointer to the SM object
*
*	p_port
*		[in] Pointer to the port object
*
*	mgrp
*		[in] Pointer to the multicast group
*
* RETURN VALUE
*	This function doesn't return a value
*/
END_C_DECLS
#endif				/* _OSM_PORT_H_ */
