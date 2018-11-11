# Agent SCs to SC

### Description

This agent translate `SCs-text` from a specified `sc-link` into `SC-code` (build equivalent construction in `sc-memory`).

**Command class:** `command_translate`

**Parameters:**
1. `sc-link`'s that should be processed

### Example

Example command with generated output:
<scg src="../../../images/kpm/scs2sc_result.gwf"></scg>

### Result

Possible result codes:

* `sc_result_ok` - translation done without any errors;
* `sc_result_error_invalid_params` - invalid parameters for an agent;
* `sc_result_invalid_state` - there were some parsing errors;
* `sc_result_error` - unknown error during agent run.
