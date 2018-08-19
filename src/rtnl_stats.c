/*
 * Copyright (c) 2018-2021 Eugene Syromyatnikov <evgsyr@gmail.com>
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "netlink_route.h"
#include "nlattr.h"
#include "print_fields.h"

#include "netlink.h"

#include <netinet/in.h>

#include <linux/if_link.h>
#include <linux/rtnetlink.h>

#include "xlat/ifstats_attr_flags.h"
#include "xlat/ifstats_attr.h"
#include "xlat/ifstats_offload_attr.h"
#include "xlat/ifstats_xstats_type_attr.h"
#include "xlat/nl_bridge_vlan_flags.h"

static bool
decode_nla_xstats_bridge(struct tcb *const tcp,
			 const kernel_ulong_t addr,
			 const unsigned int len,
			 const void *const opaque_data)
{
	struct strace_bridge_vlan_xstats {
		uint64_t rx_bytes;
		uint64_t rx_packets;
		uint64_t tx_bytes;
		uint64_t tx_packets;
		uint16_t vid;
		uint16_t flags;
		uint32_t pad2;
	} st;

	if (len < sizeof(struct strace_bridge_vlan_xstats))
		return false;

	if (umove_or_printaddr(tcp, addr, &st))
		return true;

	tprint_struct_begin();
	PRINT_FIELD_U(st, rx_bytes);
	tprint_struct_next();
	PRINT_FIELD_U(st, rx_packets);
	tprint_struct_next();
	PRINT_FIELD_U(st, tx_bytes);
	tprint_struct_next();
	PRINT_FIELD_U(st, tx_packets);
	tprint_struct_next();
	PRINT_FIELD_U(st, vid);
	tprint_struct_next();
	PRINT_FIELD_FLAGS(st, flags, nl_bridge_vlan_flags,
			  "BRIDGE_VLAN_INFO_???");
	tprint_struct_end();

	return true;
}

static const nla_decoder_t ifstats_xstats_decoders[] = {
	[LINK_XSTATS_TYPE_UNSPEC]	= NULL,
	[LINK_XSTATS_TYPE_BRIDGE]	= decode_nla_xstats_bridge,
};

static bool
decode_nla_link_xstats(struct tcb *const tcp,
		       const kernel_ulong_t addr,
		       const unsigned int len,
		       const void *const opaque_data)
{
	decode_nlattr(tcp, addr, len, ifstats_xstats_type_attr,
		      "LINK_XSTATS_TYPE_???",
		      ARRSZ_PAIR(ifstats_xstats_decoders),
		      opaque_data);

	return true;
}

static const nla_decoder_t ifstats_offload_xstats_decoders[] = {
	[LINK_XSTATS_TYPE_UNSPEC]	= NULL,
	[LINK_XSTATS_TYPE_BRIDGE]	= decode_nla_rtnl_link_stats64,
};

static bool
decode_nla_link_offload_xstats(struct tcb *const tcp,
			       const kernel_ulong_t addr,
			       const unsigned int len,
			       const void *const opaque_data)
{
	decode_nlattr(tcp, addr, len, ifstats_offload_attr,
		      "IFLA_OFFLOAD_XSTATS_???",
		      ARRSZ_PAIR(ifstats_offload_xstats_decoders),
		      opaque_data);

	return true;
}

static const nla_decoder_t ifstatsmsg_nla_decoders[] = {
	[IFLA_STATS_UNSPEC]			= NULL,
	[IFLA_STATS_LINK_64]			= decode_nla_rtnl_link_stats64,
	[IFLA_STATS_LINK_XSTATS]		= decode_nla_link_xstats,
	[IFLA_STATS_LINK_XSTATS_SLAVE]		= decode_nla_link_xstats,
	[IFLA_STATS_LINK_OFFLOAD_XSTATS]	= decode_nla_link_offload_xstats,
	[IFLA_STATS_AF_SPEC]			= decode_nla_ifla_af_spec,
};

DECL_NETLINK_ROUTE_DECODER(decode_ifstatsmsg)
{
	struct if_stats_msg ifstats = { .family = family };
	size_t offset = sizeof(ifstats.family);
	bool decode_nla = false;

	tprint_struct_begin();
	PRINT_FIELD_XVAL(ifstats, family, addrfams, "AF_???");

	if (len >= sizeof(ifstats)) {
		if (!umoven_or_printaddr(tcp, addr + offset,
					 sizeof(ifstats) - offset,
					 (char *) &ifstats + offset)) {
			if (ifstats.pad1) {
				tprint_struct_next();
				PRINT_FIELD_X(ifstats, pad1);
			}
			if (ifstats.pad2) {
				tprint_struct_next();
				PRINT_FIELD_X(ifstats, pad2);
			}
			tprint_struct_next();
			PRINT_FIELD_IFINDEX(ifstats, ifindex);
			tprint_struct_next();
			PRINT_FIELD_FLAGS(ifstats, filter_mask,
					  ifstats_attr_flags,
					  "IFLA_STATS_???");
			decode_nla = true;
		}
	} else {
		tprint_more_data_follows();
	}
	tprint_struct_end();

	offset = NLMSG_ALIGN(sizeof(ifstats));
	if (decode_nla && len > offset) {
		tprint_array_next();
		decode_nlattr(tcp, addr + offset, len - offset,
			      ifstats_attr, "IFLA_STATS_???",
			      ARRSZ_PAIR(ifstatsmsg_nla_decoders), NULL);
	}
}
