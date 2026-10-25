# GA10x GPU ID coverage (generated from kernel/gpu_unlock/main.c supported_ids[] + pci.ids snapshot)

No IDs were added or changed. "Needs hardware validation" is a documentation flag, not a claim about hardware behaviour. 10de:2216 (RTX 3080 LHR) IS listed in supported_ids[] but has no row in the README Tested-cards table.

| # | PCI ID | Chip (code comment) | Name in pci.ids (2026-10-25 snapshot) | In supported_ids[] | README 'Tested cards' entry | Needs hardware validation |
|---|---|---|---|---|---|---|
| 1 | `10de:2200` | GA102  | GA102 | yes | none | yes - no row in the README table |
| 2 | `10de:2203` | GA102  | GA102 [GeForce RTX 3090 Ti] | yes | RTX 3090 Ti: Unlocked | see README row (user reports only; not re-verified here) |
| 3 | `10de:2204` | GA102  | GA102 [GeForce RTX 3090] | yes | RTX 3090: Unlocked | see README row (user reports only; not re-verified here) |
| 4 | `10de:2205` | GA102  | GA102 [GeForce RTX 3080 Ti 20GB] | yes | RTX 3080 Ti 20 GB (OEM, GA102-250): Unlock completes, BF16 stays at half rate | see README row (user reports only; not re-verified here) |
| 5 | `10de:2206` | GA102  | GA102 [GeForce RTX 3080] | yes | RTX 3080 10 GB: Unlocked; RTX 3080 20 GB (memory upgrade): Unlocked | see README row (user reports only; not re-verified here) |
| 6 | `10de:2207` | GA102  | GA102 [GeForce RTX 3070 Ti] | yes | none | yes - no row in the README table |
| 7 | `10de:2208` | GA102  | GA102 [GeForce RTX 3080 Ti] | yes | none | yes - no row in the README table |
| 8 | `10de:220a` | GA102  | GA102 [GeForce RTX 3080 12GB] | yes | none | yes - no row in the README table |
| 9 | `10de:220d` | GA102  | GA102 [CMP 90HX] | yes | none | yes - no row in the README table |
| 10 | `10de:2216` | GA102  | GA102 [GeForce RTX 3080 Lite Hash Rate] | yes | none | yes - no README row; issues #66/#73 (from the ticket) are user reports, not verified here |
| 11 | `10de:222b` | GA102  | GA102 [GeForce RTX 3090 Engineering Sample] | yes | none | yes - no row in the README table |
| 12 | `10de:222f` | GA102  | GA102 [GeForce RTX 3080 11GB / 12GB Engineering Sample] | yes | none | yes - no row in the README table |
| 13 | `10de:2480` | GA104  | GA104 [Reserved Dev ID A] | yes | none | yes - no row in the README table |
| 14 | `10de:2482` | GA104  | GA104 [GeForce RTX 3070 Ti] | yes | RTX 3070 Ti: Not tested with this module (the same unlock worked from a UEFI port on Windows) | see README row (user reports only; not re-verified here) |
| 15 | `10de:2483` | GA104  | GA104 | yes | none | yes - no row in the README table |
| 16 | `10de:2484` | GA104  | GA104 [GeForce RTX 3070] | yes | RTX 3070: Unlocked | see README row (user reports only; not re-verified here) |
| 17 | `10de:2486` | GA104  | GA104 [GeForce RTX 3060 Ti] | yes | none | yes - no row in the README table |
| 18 | `10de:2487` | GA104  | GA104 [GeForce RTX 3060] | yes | none | yes - no row in the README table |
| 19 | `10de:2488` | GA104  | GA104 [GeForce RTX 3070 Lite Hash Rate] | yes | none | yes - no row in the README table |
| 20 | `10de:2489` | GA104  | GA104 [GeForce RTX 3060 Ti Lite Hash Rate] | yes | none | yes - no row in the README table |
| 21 | `10de:248a` | GA104  | GA104 [CMP 70HX] | yes | none | yes - no row in the README table |
| 22 | `10de:248c` | GA104  | GA104 [GeForce RTX 3070 Ti] | yes | none | yes - no row in the README table |
| 23 | `10de:248d` | GA104  | GA104 [GeForce RTX 3070] | yes | none | yes - no row in the README table |
| 24 | `10de:248e` | GA104  | GA104 [GeForce RTX 3060 Ti] | yes | none | yes - no row in the README table |
| 25 | `10de:249c` | GA104  | GA104M [GeForce RTX 3080 Mobile / Max-Q 8GB/16GB] | yes | none | yes - no row in the README table |
| 26 | `10de:249d` | GA104  | GA104M [GeForce RTX 3070 Mobile / Max-Q] | yes | none | yes - no row in the README table |
| 27 | `10de:249f` | GA104  | GA104M | yes | none | yes - no row in the README table |
| 28 | `10de:24a0` | GA104  | GA104 [Geforce RTX 3070 Ti Laptop GPU] | yes | none | yes - no row in the README table |
| 29 | `10de:24a4` | GA104  | GA104M | yes | none | yes - no row in the README table |
| 30 | `10de:24ac` | GA104  | GA104 [GeForce RTX 30x0 Engineering Sample] | yes | none | yes - no row in the README table |
| 31 | `10de:24ad` | GA104  | GA104 [GeForce RTX 3060 Engineering Sample] | yes | none | yes - no row in the README table |
| 32 | `10de:24af` | GA104  | GA104 [GeForce RTX 3070 Engineering Sample] | yes | none | yes - no row in the README table |
| 33 | `10de:24bf` | GA104  | GA104 [GeForce RTX 3070 Engineering Sample] | yes | none | yes - no row in the README table |
| 34 | `10de:24c0` | GA104  | GA104 [Initial Dev ID B] | yes | none | yes - no row in the README table |
| 35 | `10de:24c7` | GA104  | GA104 [GeForce RTX 3060 8GB] | yes | none | yes - no row in the README table |
| 36 | `10de:24c8` | GA104  | GA104 [GeForce RTX 3070 GDDR6X] | yes | none | yes - no row in the README table |
| 37 | `10de:24c9` | GA104  | GA104 [GeForce RTX 3060 Ti GDDR6X] | yes | none | yes - no row in the README table |
| 38 | `10de:24dc` | GA104  | GA104M [GeForce RTX 3080 Mobile / Max-Q 8GB/16GB] | yes | none | yes - no row in the README table |
| 39 | `10de:24dd` | GA104  | GA104M [GeForce RTX 3070 Mobile / Max-Q] | yes | none | yes - no row in the README table |
| 40 | `10de:24df` | GA104  | GA104M | yes | none | yes - no row in the README table |
| 41 | `10de:24e0` | GA104  | GA104M [Geforce RTX 3070 Ti Laptop GPU] | yes | none | yes - no row in the README table |
| 42 | `10de:2414` | GA103 (untested) | GA103 [GeForce RTX 3060 Ti] | yes | any: No reports yet | see README row (user reports only; not re-verified here) |
| 43 | `10de:2501` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3060] | yes | none | yes - no row in the README table |
| 44 | `10de:2503` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3060] | yes | none | yes - no row in the README table |
| 45 | `10de:2504` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3060 Lite Hash Rate] | yes | RTX 3060 12 GB: Unlocked | see README row (user reports only; not re-verified here) |
| 46 | `10de:2505` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 | yes | none | yes - no row in the README table |
| 47 | `10de:2507` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [Geforce RTX 3050] | yes | none | yes - no row in the README table |
| 48 | `10de:2508` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3050 OEM] | yes | none | yes - no row in the README table |
| 49 | `10de:2509` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3060 12GB Rev. 2] | yes | none | yes - no row in the README table |
| 50 | `10de:252f` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3060 Engineering Sample] | yes | none | yes - no row in the README table |
| 51 | `10de:2544` | GA106 (validated only on RTX 3060 12GB 0x2504) | GA106 [GeForce RTX 3060] | yes | none | yes - no row in the README table |

Total IDs in supported_ids[]: 51; with a README row: 8; README rows whose ID is not in supported_ids[]: []
