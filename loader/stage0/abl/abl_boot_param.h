/****************************************************************************
* Copyright (c) 2015 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0

* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/

#ifndef _ABL_BOOT_PARAM_H_
#define _ABL_BOOT_PARAM_H_

#include "stage0_asm.h"
#include "evmm_desc.h"
#include "trusty_info.h"

#define IMAGE_ID_MAX_LEN 8

#define IMAGE_BOOT_PARAMS_VERSION   1
#define VMM_BOOT_PARAMS_VERSION     1
#define TRUSTY_BOOT_PARAMS_VERSION  1
#define ANDROID_BOOT_PARAMS_VERSION 1

#define VMM_IMAGE_ID            0x00000000004d4d56ULL /* "MMV" standards for "VMM" */
#define TRUSTY_IMAGE_ID         0x0000797473757254ULL /* "ytsurT" standards for "Trusty" */
#define ANDROID_IMAGE_ID        0x0064696f72646e41ULL /* "diordnA" standards for "Android" */

typedef struct {
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
} aout_t; /* a.out kernel image */

typedef struct {
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
} elf_t; /* elf kernel */

/*only used partial of the standard multiboot_info_t, since we only need flags and cmdline */
typedef struct {
	uint32_t flags;

	/* valid if flags[0] (MBI_MEMLIMITS) set */
	uint32_t mem_lower;
	uint32_t mem_upper;

	/* valid if flags[1] set */
	uint32_t boot_device;

	/* valid if flags[2] (MBI_CMDLINE) set */
	uint32_t cmdline;

	/* valid if flags[3] (MBI_MODS) set */
	uint32_t mods_count;
	uint32_t mods_addr;

	/* valid if flags[4] or flags[5] set */
	union {
		aout_t aout_image;
		elf_t elf_image;
	} syms;

	/* valid if flags[6] (MBI_MEMMAP) set */
	uint32_t mmap_length;
	uint32_t mmap_addr;
} multiboot_info_t;

typedef struct {
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
}PACKED multiboot_memory_map_t;

/* register value ordered by: pushal, pushfl */
typedef struct init_register {
	uint32_t eflags;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
} init_register_t;

typedef enum {
	IMAGE_AOSP,
	IMAGE_ELF,
	IMAGE_OUT,
	IMAGE_MULTIBOOT,
	IMAGE_PROPRIETARY
} image_format_t;

typedef struct {
	char ImageID[IMAGE_ID_MAX_LEN];    /* "Android", "VMM", "Trusty", etc... */
	image_format_t ImageFormat;        /* AOSP, elf, a.out, multiboot, etc... */
	uint32_t ImageDataSize;            /* size of image's boot params */
	uint32_t ImageDataPtr;             /* 32-bit pointer to image's boot params */
} image_element_t;

typedef struct {
	uint16_t Version;                  /* version of this structure */
	uint16_t NbImage;                  /* num of images */
	uint32_t ImageElementAddr;         /* address of list of image_element_t */
} image_boot_params_t;

typedef struct {
	uint16_t Version;                  /* version of this structure */
	uint32_t VMMheapAddr;              /* ABL shall alloc a 36MB heap for VMM */
	uint32_t VMMheapSize;              /* heap size allocated */
	uint32_t VMMSipiApWkupAddr;        /* ABL shall alloc 1 page under 1MB mem */
	uint32_t VMMMemBase;               /* VMM's Base address */
	uint32_t VMMMemSize;               /* assumed to be 4MB */
}PACKED vmm_boot_params_t;

typedef struct {
	uint16_t Version;                  /* version of this structure */
	uint32_t TrustyMemBase;            /* Trusty mem base address */
	uint32_t TrustyMemSize;            /* assumed to be 16MB */
}PACKED trusty_boot_params_t;

typedef struct {
	uint64_t base;
	uint32_t limit;
	uint32_t attributes;
	uint16_t selector;
	uint16_t reserved[3];
} segment_struct_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
	uint32_t reserved;
}PACKED idt_gdt_register_t;

typedef struct {
	uint64_t cpu_gp_register[16]; /* rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8..r15 */
	uint64_t rip;
	uint64_t rflags;
	idt_gdt_register_t gdtr;
	idt_gdt_register_t idtr;
	segment_struct_t cs;
	segment_struct_t ds;
	segment_struct_t ss;
	segment_struct_t es;
	segment_struct_t fs;
	segment_struct_t gs;
	segment_struct_t ldtr;
	segment_struct_t tr;
	uint32_t cr0;
	uint32_t cr3;
	uint32_t cr4;
	uint32_t cr8;
	uint64_t msr_efer;
}PACKED cpu_state_t;

typedef struct {
	uint16_t Version;                              /* version of this structure */
	uint16_t ImagePreload;
	union {
		cpu_state_t CpuState;                  /* used in case ImagePreload is true */
		struct {                               /* used in case ImagePreload is false */
			uint32_t OemKeyManifestAddr;
			uint32_t OemKeyManifestSize;
			uint32_t AndroidCmdLine;
		} AndroidImageUnpreload;
	};
} android_image_boot_params_t;

evmm_desc_t *boot_params_parse(const init_register_t *init_reg);
#endif