/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include <FreeRTOS.h>
#include <FreeRTOS_CLI.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef CONFIG_ARM64
#include "initcall.h"
#endif

static int btstack_subcmd_handler(size_t argc, char **argv)
{
	printf("%s, argc=%ld\n", __func__, argc);

	if (!strcmp(argv[0], "on"))
		btstack_go();
	else if (!strcmp(argv[0], "off"))
		printf("%s, not implement\n", __func__, argc);
	return 0;
}

static const struct cli_subcmd btstack_subcmd[] = {
	{
	"on",
	"btstack start",
	btstack_subcmd_handler,
	},
	{
	"off",
	"btstack stop",
	btstack_subcmd_handler,
	},
	{
	NULL,
	},
};

static const CLI_Command_Definition_t btstack_shell_command = {
	"btstack",
	btstack_subcmd,
	"Usage: btstack [SUBCOMMAND] [OPTION]...\n",
	NULL,
	-1 /* The user can enter any number of commands. */
};

#ifdef CONFIG_ARM64
CMD_INIT(btstack_shell_command);
#endif
