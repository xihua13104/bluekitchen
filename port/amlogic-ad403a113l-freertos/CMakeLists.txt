# Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.

# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)
if (CONFIG_BTSTACK)
	aml_add_library()
	set( 3RD_PARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd-party)
	set( CHIPSET_DIR ${CMAKE_CURRENT_LIST_DIR}/chipset)
	set( EXAMPLE_DIR ${CMAKE_CURRENT_LIST_DIR}/example)
	set( PLATFORM_DIR ${CMAKE_CURRENT_LIST_DIR}/platform)
	set( PORT_DIR ${CMAKE_CURRENT_LIST_DIR}/port/amlogic-ad403-freerots)
	set( SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

	aml_library_include_directories("${SRC_DIR}/ble")
	aml_library_include_directories("${SRC_DIR}/ble/gatt-service")
	aml_library_include_directories("${SRC_DIR}/classic")
	aml_library_include_directories("${SRC_DIR}")
	aml_library_include_directories("${3RD_PARTY_DIR}/micro-ecc")
	aml_library_include_directories("${3RD_PARTY_DIR}/bluedroid/decoder/include")
	aml_library_include_directories("${3RD_PARTY_DIR}/bluedroid/encoder/include")
	aml_library_include_directories("${3RD_PARTY_DIR}/hxcmod-player")
	aml_library_include_directories("${3RD_PARTY_DIR}/hxcmod-player/mods")
	#aml_library_include_directories("${3RD_PARTY_DIR}/lwip/core/src/include")
	#aml_library_include_directories("${3RD_PARTY_DIR}/lwip/dhcp-server")
	aml_library_include_directories("${3RD_PARTY_DIR}/md5")
	aml_library_include_directories("${3RD_PARTY_DIR}/yxml")
	aml_library_include_directories("${3RD_PARTY_DIR}/segger-rtt")

	aml_library_include_directories("${PLATFORM_DIR}/embedded")
	aml_library_include_directories("${PLATFORM_DIR}/freertos")
	#aml_library_include_directories("${PLATFORM_DIR}/lwip")
	#aml_library_include_directories("${PLATFORM_DIR}/lwip/port")
	aml_library_include_directories("${PORT_DIR}/port")

	aml_library_sources(
	"${SRC_DIR}/ad_parser.c"
	"${SRC_DIR}/ble/att_db.c"
	"${SRC_DIR}/ble/att_dispatch.c"
	"${SRC_DIR}/ble/att_server.c"
	"${SRC_DIR}/ble/gatt-service/ancs_client.c"
	"${SRC_DIR}/ble/gatt-service/battery_service_client.c"
	"${SRC_DIR}/ble/gatt-service/battery_service_server.c"
	"${SRC_DIR}/ble/gatt-service/device_information_service_client.c"
	"${SRC_DIR}/ble/gatt-service/device_information_service_server.c"
	"${SRC_DIR}/ble/gatt-service/hids_device.c"
	"${SRC_DIR}/ble/gatt-service/nordic_spp_service_server.c"
	"${SRC_DIR}/ble/gatt_client.c"
	"${SRC_DIR}/ble/le_device_db_memory.c"
	"${SRC_DIR}/ble/le_device_db_tlv.c"
	"${SRC_DIR}/ble/sm.c"

	"${SRC_DIR}/classic/a2dp_sink.c"
	"${SRC_DIR}/classic/a2dp_source.c"
	"${SRC_DIR}/classic/avdtp.c"
	"${SRC_DIR}/classic/avdtp_acceptor.c"
	"${SRC_DIR}/classic/avdtp_initiator.c"
	"${SRC_DIR}/classic/avdtp_sink.c"
	"${SRC_DIR}/classic/avdtp_source.c"
	"${SRC_DIR}/classic/avdtp_util.c"
	"${SRC_DIR}/classic/avrcp.c"
	"${SRC_DIR}/classic/avrcp_browsing_controller.c"
	"${SRC_DIR}/classic/avrcp_controller.c"
	"${SRC_DIR}/classic/avrcp_media_item_iterator.c"
	"${SRC_DIR}/classic/avrcp_target.c"
	"${SRC_DIR}/classic/bnep.c"
	"${SRC_DIR}/classic/btstack_cvsd_plc.c"
	"${SRC_DIR}/classic/btstack_link_key_db_tlv.c"
	"${SRC_DIR}/classic/btstack_sbc_decoder_bluedroid.c"
	"${SRC_DIR}/classic/btstack_sbc_encoder_bluedroid.c"
	"${SRC_DIR}/classic/btstack_sbc_plc.c"
	"${SRC_DIR}/classic/device_id_server.c"
	"${SRC_DIR}/classic/goep_client.c"
	"${SRC_DIR}/classic/hfp.c"
	"${SRC_DIR}/classic/hfp_ag.c"
	"${SRC_DIR}/classic/hfp_gsm_model.c"
	"${SRC_DIR}/classic/hfp_hf.c"
	"${SRC_DIR}/classic/hfp_msbc.c"
	"${SRC_DIR}/classic/hid_device.c"
	"${SRC_DIR}/classic/hid_host.c"
	"${SRC_DIR}/classic/hsp_ag.c"
	"${SRC_DIR}/classic/hsp_hs.c"
	"${SRC_DIR}/classic/obex_iterator.c"
	"${SRC_DIR}/classic/obex_message_builder.c"
	"${SRC_DIR}/classic/pan.c"
	"${SRC_DIR}/classic/pbap_client.c"
	"${SRC_DIR}/classic/rfcomm.c"
	"${SRC_DIR}/classic/gatt_sdp.c"
	"${SRC_DIR}/classic/sdp_client.c"
	"${SRC_DIR}/classic/sdp_client_rfcomm.c"
	"${SRC_DIR}/classic/sdp_server.c"
	"${SRC_DIR}/classic/sdp_util.c"
	"${SRC_DIR}/classic/spp_server.c"

	"${SRC_DIR}/btstack_audio.c"
	"${SRC_DIR}/btstack_resample.c"
	"${SRC_DIR}/btstack_crypto.c"
	"${SRC_DIR}/btstack_hid_parser.c"
	"${SRC_DIR}/btstack_linked_list.c"
	"${SRC_DIR}/btstack_memory.c"
	"${SRC_DIR}/btstack_memory_pool.c"
	"${SRC_DIR}/btstack_ring_buffer.c"
	"${SRC_DIR}/btstack_run_loop.c"
	"${SRC_DIR}/btstack_tlv.c"
	"${SRC_DIR}/btstack_util.c"
	"${SRC_DIR}/hci.c"
	"${SRC_DIR}/hci_cmd.c"
	"${SRC_DIR}/hci_dump.c"
	"${SRC_DIR}/hci_transport_h4.c"
	"${SRC_DIR}/l2cap.c"
	"${SRC_DIR}/l2cap_signaling.c"

	"${3RD_PARTY_DIR}/hxcmod-player/hxcmod.c"
	"${3RD_PARTY_DIR}/hxcmod-player/mods/nao-deceased_by_disease.c"
	"${3RD_PARTY_DIR}/micro-ecc/uECC.c"
	#"${3RD_PARTY_DIR}/segger-rtt/SEGGER_RTT.c"
	#"${3RD_PARTY_DIR}/segger-rtt/SEGGER_RTT_printf.c"
	#"${3RD_PARTY_DIR}/segger-rtt/SEGGER_RTT_Syscalls_GCC.c"
	"${3RD_PARTY_DIR}/yxml/yxml.c"
	"${3RD_PARTY_DIR}/md5/md5.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/def.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/inet_chksum.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/init.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ip.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/mem.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/memp.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/netif.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/pbuf.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/tcp.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/tcp_in.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/tcp_out.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/timeouts.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/udp.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/acd.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/dhcp.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/etharp.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/icmp.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/ip4.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/ip4_addr.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/core/ipv4/ip4_frag.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/netif/ethernet.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/apps/http/altcp_proxyconnect.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/apps/http/fs.c"
	#"${3RD_PARTY_DIR}/lwip/core/src/apps/http/httpd.c"
	#"${3RD_PARTY_DIR}/lwip/dhcp-server/dhserver.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/alloc.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/bitalloc.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/bitalloc-sbc.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/bitstream-decode.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/decoder-oina.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/decoder-private.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/decoder-sbc.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/dequant.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/framing.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/framing-sbc.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/oi_codec_version.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/synthesis-sbc.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/synthesis-dct8.c"
	"${3RD_PARTY_DIR}/bluedroid/decoder/srce/synthesis-8-generated.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_analysis.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_dct.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_dct_coeffs.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_enc_bit_alloc_mono.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_enc_bit_alloc_ste.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_enc_coeffs.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_encoder.c"
	"${3RD_PARTY_DIR}/bluedroid/encoder/srce/sbc_packing.c"

	"${PLATFORM_DIR}/freertos/btstack_run_loop_freertos.c"
	"${PLATFORM_DIR}/freertos/btstack_uart_block_freertos.c"
	"${PLATFORM_DIR}/embedded/hci_dump_embedded_stdout.c"

	"${EXAMPLE_DIR}/sco_demo_util.c"
	"${EXAMPLE_DIR}/headset_demo.c"
	#"${PLATFORM_DIR}/lwip/port/sys_arch.c"
	#"${PLATFORM_DIR}/lwip/bnep_lwip.c"

	"${PORT_DIR}/port/btstack_port.c"
	)



if (CONFIG_CLI)
	aml_library_sources(
	"${PORT_DIR}/port/btstack_shell.c"
	)
	aml_library_link_libraries(lib__cli)
endif()

	aml_library_link_libraries(wcn__bt_drv)
	#aml_library_link_libraries(wcn__blueaml)
	#aml_library_link_libraries(lib__ulog)
	aml_library_link_libraries(kernel__${KERNEL})
	aml_link_libraries()
endif()