#ifndef __BSP_CFG_H__
#define __BSP_CFG_H__

/** Assertion / logging mode. See hal_common_api.h for the semantics of each value. */
#define BSP_CFG_ASSERT                    (1)

/** Default parameter-checking state for every driver in this project.
 *  Per-driver cfg headers pick this up unless they override explicitly. */
#define BSP_CFG_PARAM_CHECKING_ENABLE     (1)

#endif // __BSP_CFG_H__
