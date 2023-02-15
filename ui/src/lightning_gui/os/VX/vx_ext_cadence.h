/*
 * Copyright (c) 2015-2017 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.
 *
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Cadence Design
 * Systems Inc. They may be adapted and modified by bona fide purchasers for
 * internal use, but neither the original nor any adapted or modified version
 * may be disclosed or distributed to third parties in any manner, medium, or
 * form, in whole or in part, without the prior written consent of Cadence
 * Design Systems Inc or in accordance with the terms of your Design Technology
 * License Agreement with Cadence Design Systems. This software and its
 * derivatives are to be executed solely on products incorporating a Cadence
 * Design Systems processor.
 */

#ifndef _VX_EXT_CADENCE_H_INCLUDED_
#define _VX_EXT_CADENCE_H_INCLUDED_

#include <VX/vx.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TENVX_NR_CORES
#define TENVX_NR_CORES 2
#endif

/*! \brief The Xtensa Target Enumeration.
 * \ingroup group_target
 */
typedef enum tenvx_target_e {
    /*! \brief Start of Vendor specific target enumerates. */
    TENVX_TARGET_NONE = VX_TARGET_VENDOR_BEGIN + 0x0,
    TENVX_TARGET_HOST_C = VX_TARGET_VENDOR_BEGIN + 0x1,
    TENVX_TARGET_VISION_XI = VX_TARGET_VENDOR_BEGIN + 0x2,
    TENVX_TARGET_MIXED =
        VX_TARGET_VENDOR_BEGIN +
        0x4,  // Indicates that a node is implemented by multiple kernels and
              // they run on different targets.
} tenvx_target_e;

/*! \brief Cadence specific Node Attribute Enumerations.
 * \ingroup group_node
 */

typedef enum tenvx_node_attribute_e {
    /*! \brief Indicates where the node is executed. Read-only.
     * Use a <tt>\ref vx_uint32</tt> parameter.
     */
    TENVX_NODE_EXEC_TARGET_ID =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_NODE) + 0x0,

    /* Reserved for VCK : TENVX_NODE_LOCAL_DATA_SIZE_2 */
    __TENVX_NODE_LOCAL_DATA_SIZE_2__ =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_NODE) + 0x1,

    /*! \brief Indicates last node attribute extension.
     */
    TENVX_NODE_ATTR_END,
} tenvx_node_attribute_e;

typedef enum _TENVX_LOG_MODE {
    TENVX_LOG_MODE_NONE = 0,
    TENVX_LOG_MODE_CONSOLE,
    TENVX_LOG_MODE_BUFFER
} TENVX_LOG_MODE;

typedef enum _TENVX_LOG_SEVERITY {
    TENVX_LOG_SEVERITY_GRAPH_ERROR = 0,
    TENVX_LOG_SEVERITY_ERROR,
    TENVX_LOG_SEVERITY_WARNING,
    TENVX_LOG_SEVERITY_DEBUG,
    TENVX_LOG_SEVERITY_DETAILED
} TENVX_LOG_SEVERITY;

typedef enum _TENVX_LOG_REPORT {
    TENVX_LOG_REPORT_CLEAR = 0,
    TENVX_LOG_REPORT_NEXT,
    TENVX_LOG_REPORT_PREV
} TENVX_LOG_REPORT;

typedef struct _tenvx_log_control {
    unsigned mode;
    unsigned severity;
    int max_buffer_size;
} tenvx_hint_log_t;

typedef struct _tenvx_hint_value {
    unsigned value;
} tenvx_hint_value_t;

typedef struct _tenvx_hint_tile_size {
    unsigned height;
    unsigned width;
} tenvx_hint_tile_size_t;

typedef struct _tenvx_hint_mem_reserve {
    unsigned core_id;
    unsigned dmem0_reserve;
    unsigned dmem1_reserve;
} tenvx_hint_mem_reserve_t;

typedef struct _tenvx_hint_target {
    tenvx_target_e target;
    vx_bool valid;
    vx_bool disable_all_libs;
} tenvx_hint_target_t;

/*! \brief Cadence specific hints.
 * \see <tt>vxHint</tt>
 * \ingroup group_hint
 */

enum tenvx_hint_e {
    /*! \brief Suggest where context/graph should execute */
    TENVX_HINT_TARGET = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x0,
    /*! \brief  */
    TENVX_HINT_IMG_PITCH = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x1,
    /*! \brief  */
    TENVX_HINT_TILE_SIZE = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x2,
    /*! \brief  */
    TENVX_HINT_ARR_MEM = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x3,
    /*! \brief  */
    TENVX_FORCE_TARGET = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x4,
    /*! \brief  */
    TENVX_FORCE_IMG_PITCH = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x5,
    /*! \brief  */
    TENVX_FORCE_TILE_SIZE = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x6,
    /*! \brief  */
    TENVX_FORCE_ARR_MEM = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x7,
    /*! \brief  */
    TENVX_FORCE_LOG = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x8,
    /*! \brief  */
    TENVX_FORCE_DMEM_RESERVE = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x9,
    /*! \brief */
    TENVX_INT_FORCE_XI_MAP = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x10,
    /*! \brief  */
    TENVX_INT_FORCE_SKIP_GM = VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x11,
    /*! \brief  */
    TENVX_INT_FORCE_OPT_DISABLE =
        VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x12,
    /*! \brief  */
    TENVX_INT_FORCE_OPT_TILE_SHARE =
        VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x13,
    /*! \brief  */
    TENVX_INT_FORCE_OPT_SPLIT =
        VX_ENUM_BASE(VX_ID_CADENCE, VX_ENUM_HINT) + 0x14,
};

/*
 * Functions and types
 *
 */

typedef void *tenvx_log_info;

/* Function: tenvx_graph_mapper_print_all_logs
 *
 * Helper function to iterate over the log buffer and print all of the logs.
 *
 */
void tenvx_graph_mapper_print_all_logs();

/* Function: xi_graph_mapper_report_log
 *
 * Access the logged logs when using TENVX_LOG_MODE_BUFFER.
 *
 * Parameters:
 *  err: An indicator of the last access to the log buffer.
 *  req: The following behaviour will be observed
 *      TENVX_LOG_REPORT_CLEAR      - Clear the log buffer, returns NULL.
 *      TENVX_LOG_REPORT_NEXT       - Returns the next valid report. If the err
 * param is NULL, will return the first logged report. TENVX_LOG_REPORT_PREV   -
 * Returns the prev valid report. If the err param is NULL, will return the
 * first logged report.
 */
tenvx_log_info tenvx_graph_mapper_log_iterate(tenvx_log_info err,
                                              TENVX_LOG_REPORT req);

/* Function: tenvx_graph_mapper_log_string
 * Get an log string describing the log.
 */
char *tenvx_graph_mapper_log_string(tenvx_log_info err);

/* Function: tenvx_graph_mapper_log_code
 * Get an log code describing the log.
 * This log code corresponds to vx_status with IVP specific extensions
 */

int tenvx_graph_mapper_log_code(tenvx_log_info err);

/* Function: tenvx_graph_mapper_log_code
 * Get a severity code describing the log.
 */

TENVX_LOG_SEVERITY tenvx_graph_mapper_log_severity(tenvx_log_info err);

/* ===========================================================================
 *
 * Supported OpenVX 1.2 functionalities
 *
 * ===========================================================================
 */

vx_status VX_API_CALL
vxSetImagePixelValues(vx_image image, const vx_pixel_value_t *pixel_value);

/* ===========================================================================
 *
 * Vendor Custom kernels (VCK host API)
 *
 * ===========================================================================
 */

/* Additional data object type */

/*! \brief VCK specific type to be used with <tt>\ref
 * vxAddParameterToKernel</tt> \ingroup group_user_kernels
 */
enum tenvx_vck_type_e {
    /*! \brief Declares a tilable image as parameter.
     * A tilable image has the same data type as an OpenVX image. When querying
     * the kernel parameter, <tt>\ref VX_TYPE_IMAGE</tt> will be returned.
     * <tt>\refTENVX_TYPE_IMAGE_TILABLE</tt> simply informs that the VCK kernel
     * DSP callbacks are written to handle this parameter as a tile in local
     * (dmem) memory, rather than a full image in system memory..
     */
    TENVX_TYPE_IMAGE_TILABLE = VX_TYPE_VENDOR_OBJECT_START + 0
};

/*! \brief VCK specific enumeration to be used with <tt>\ref
 * vxSetKernelAttribute</tt>. \ingroup group_user_kernels
 */
enum tenvx_vck_kernel_attribute_e {
    /* Kernel attribute identifiers to be used with vxSetKernelAttribute */

    /*! \brief Request and enforce a specific border mode for the kernel.
     * Write-only Use a <tt>\ref vx_border_t</tt> parameter. When a border mode
     * is required, the application will not be allowed to change the border
     * mode of nodes instantiated from the kernel. Note: when no kernel border
     * mode information is set, the border mode is by default <tt>\ref
     * VX_BORDER_UNDEFINED/tt> and can be changed by the user.
     */
    TENVX_KERNEL_REQ_BORDER =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_KERNEL) + 0x0,

    /*! \brief Set the default border mode for the kernel. write-only.
     *         Use a <tt>\ref vx_border_t</tt> parameter.
     * When a default border mode is specified, nodes instantiated from the
     * kernel will have this border mode by default. The application is still
     * allowed to change the border mode of nodes. Note: when no kernel border
     * mode information is set, the border mode is by default <tt>\ref
     * VX_BORDER_UNDEFINED/tt> and can be changed by the user.
     */
    TENVX_KERNEL_DEFAULT_BORDER =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_KERNEL) + 0x1,

    /*! \brief For VCK kernel, it specifies the size in bytes of memory
     * requested by the kernel for the second buffer in DSP local memory (dmem).
     * Read-write attribute; can be written only before user-kernel finalization
     * and if <tt>\refVX_KERNEL_LOCAL_DATA_SIZE</tt> was previously set with a
     * non-zero value. Use a vx_size parameter. If not set it will default to
     * zero. Notes:
     * - When a kernel local buffer size is set, it indicates that the buffer
     * size is independent of the parameters to nodes instantiated from the
     * kernel. If the size of the local buffer depends on meta-data of derive
     * node parameters, this attribute should not be set, and instead <tt>\ref
     * TENVX_NODE_LOCAL_DATA_SIZE_2</tt> should be set in the node
     * initialization callback.
     * - The DSP local memory is usually small (e.g., 128KB per dmem bank) and
     * used for multiple purposes (e.g., stack, runtime data, kernel data,
     * command script). Setting a too large value may result in a graph
     * verification error, when the graph contains a node instantiated from a
     * VCK that requests a large memory buffer. In general, since the DSP dmem
     * memory is shared between software components, and since the tiling
     * optimization is more efficient when multiple kernels can reside on the
     * DSP at the same time, it is recommended to minimize as much as possible
     *   the amount of memory requested by kernels.
     */
    TENVX_KERNEL_LOCAL_DATA_SIZE_2 =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_KERNEL) + 0x2,

    /*! \brief Inform about how many explicit and concurrent DMA requests will
     * be done by the kernel user code. Read-Write. Use a vx_uint32 parameter.
     * By default, the value is zero. Notes:
     * - This attribute is a hint. Setting it only impacts the performance, not
     * the functionality.
     * - Setting this attribute is only relevant for VCK kernels with full image
     * parameters, or arrays that will not be requested in local memory. In this
     * case, the user code may use the DMA API to access data.
     */
    TENVX_KERNEL_HINT_DMA_QUEUE =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_KERNEL) + 0x3,
};

/*! \brief VCK specific enumeration to be used with <tt>\ref
 * tevnxSetKernelParamAttribute</tt>. \ingroup group_user_kernels
 */
enum tenvx_vck_parameter_attribute_e {
    /* Attribute identifiers to be used tenvxSetKernelParamAttribute*/

    /*! \brief Requests an array parameter to be automatically made available in
     * DSP local memory by OpenVX. Write-only. Use a vx_bool parameter.
     */
    TENVX_KERNEL_PARAM_REQ_LOCAL =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_PARAMETER) + 0x0,

    /*! \brief Specifies the requested tile neighborhood area, in pixels.
     * Write-only. Use a <tt>\ref tenvx_neighborhood_size_t</tt> parameter. This
     * option is only available for <tt>\ref TENVX_TYPE_IMAGE_TILABLE</tt>
     * parameters. The neighborhood area must be symmetric, i.e., right size
     * equals left size, and top size equals bottom size.
     */
    TENVX_KERNEL_PARAM_TILE_NEIGHBORHOOD =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_KERNEL) + 0x1,
};

/*! \brief VCK specific enumeration to be used with <tt>\ref
 * tenvxSetNodeAttribute</tt>. \ingroup group_user_kernels
 */
enum tenvx_vck_node_attribute_e {
    /*! \brief It specifies the size in bytes of memory requested by the kernel
     * for the second buffer in DSP local memory (dmem). Read-Write attribute.
     * Use a vx_size parameter. Can be written only at user-node
     * (de)initialization if VX_NODE_LOCAL_DATA_SIZE!=0 (size set at kernel
     * registration time with VX_KERNEL_LOCAL_DATA_SIZE or with
     * VX_NODE_LOCAL_DATA_SIZE at node initialization time) and
     * VX_KERNEL_LOCAL_DATA_SIZE_2==0. Notes:
     * - When setting a node local buffer size, it usually means that the size
     * is dependent from the meta-data of at least one of the node parameters.
     * If this size is constant and independent from node parameters, then
     * <tt>\ref TENVX_KERNEL_LOCAL_DATA_SIZE_2</tt> should be set instead during
     * kernel registration.
     * - The DSP local memory is usually small (e.g., 128KB per dmem bank) and
     * used for multiple purposes (e.g., stack, runtime data, kernel data,
     * command script). Setting a too large value may result in a graph
     * verification error, when the graph contains a node instantiated from a
     * VCK that requests a large memory buffer. In general, since the DSP dmem
     * memory is shared between software components, and since the tiling
     * optimization is more efficient when multiple kernels can reside on the
     * DSP at the same time, it is recommended to minimize as much as possible
     * the amount of memory requested by kernels.
     */
    TENVX_NODE_LOCAL_DATA_SIZE_2 =
        VX_ATTRIBUTE_BASE(VX_ID_CADENCE, VX_TYPE_NODE) + 0x1,
};

/*! \brief Particular values for handling DSP kernel callback IDs
 * \ingroup group_user_kernels
 */
enum tenvx_vck_kernel_index_e {
    /*! \brief Maximum index for a DSP callback in a kernel library.
     * This is the maximum number that can be added to a kernel library ID to
     * create a DSP kernel callback ID in this kernel library.
     */
    TENVX_KERNEL_MAX_KERNEL_INDEX = 4094,

    /*! \brief Specific DSP kernel callback ID to pass to <tt>\ref
     * tenvxAddVCKernel</tt in order to specify that there is no start or end
     * DSP callback.
     */
    TENVX_KERNEL_NO_ENTRY = TENVX_KERNEL_MAX_KERNEL_INDEX + 1,
};

/*! \brief Data structure describing the neighborhood requirements for tilable
 * image parameters. \ingroup group_user_kernels
 */
typedef struct _tenvx_neighborhood_size_t {
    vx_uint32 left; /*!< \brief the number of neighborhood pixels on the left */
    vx_uint32
        right;     /*!< \brief the number of neighborhood pixels on the right */
    vx_uint32 top; /*!< \brief the number of neighborhood pixels on the top */
    vx_uint32
        bottom; /*!< \brief the number of neighborhood pixels on the bottom */
} tenvx_neighborhood_size_t;

/*! \brief Allows users to add VCK kernels to a context at run-time.
 * \param [in] context The reference to the context the kernel must be added to.
 * \param [in] target_id The enumerated value of the target device. Must be
 * TENVX_TARGET_VISION_XI. \param [in] name The string to use to identify the
 * kernel. Must be less than VX_MAX_KERNEL_NAME characters. \param [in]
 * kernel_process_id The DSP process callback ID. This is also the kernel ID
 * \param [in] numParams The number of parameters for this kernel.
 * \param [in] host_validate The parameter validation function address.
 * \param [in] host_init The kernel initialization function address. NULL needs
 * to be passed if there is no initialization function. \param [in] host_deinit
 * The kernel de-initialization function address. NULL needs to be passed if
 * there is no deinitialization function. \param [in] kernel_start_id The DSP
 * start callback ID. TENVX_KERNEL_NO_ENTRY needs to be passed if there is no
 * start callback. \param [in] kernel_end_id The DSP end callback ID.
 * TENVX_KERNEL_NO_ENTRY needs to be passed if there is no end callback. \return
 * A <tt>\ref vx_kernel</tt> reference. Any possible errors preventing a
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_user_kernels
 */
VX_API_ENTRY vx_kernel VX_API_CALL tenvxAddVCKernel(
    vx_context context, vx_enum target_id, const vx_char *name,
    vx_enum kernel_process_id, vx_uint32 nb_params,
    vx_kernel_validate_f host_validate,
    /* Optional parameters */
    vx_kernel_initialize_f host_init, vx_kernel_deinitialize_f host_deinit,
    vx_enum kernel_start_id, vx_enum kernel_end_id);

/*! \brief Sets kernel parameters attributes.
 * \param [in] kernel The reference to the kernel.
 * \param [in] index The index of the parameter.
 * \param [in] attribute The enumeration of the attributes. See
 * tenvx_kernel_param_attribute_e. \param [in] ptr The pointer to the location
 * from which to read the attribute. \param [in] size The size in bytes of the
 * data area indicated by ptr in bytes. \note After a kernel has been passed to
 * <tt>\ref vxFinalizeKernel</tt>, no attributes can be altered. \return A
 * <tt>\ref vx_status_e</tt> enumeration. \retval VX_SUCCESS No errors; any
 * other value indicates failure. \ingroup group_user_kernels
 */
VX_API_ENTRY vx_status VX_API_CALL
tenvxSetKernelParamAttribute(vx_kernel kernel, vx_uint32 index,
                             vx_enum attribute, const void *ptr, vx_size size);

/*! Registers a kernel module to an OpenVX context.
 * \param [in] context The reference to the context.
 * \param [in] module_name The name that identified the kernel library.
 * \param [in] publish The address of the kernel publish callback function.
 * \param [in] unpublish The address of the kernel un-publish callback function.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors; any other value indicates failure.
 */
VX_API_ENTRY vx_status VX_API_CALL tenvxRegisterKernelLibrary(
    vx_context context, const char *module_name, vx_publish_kernels_f publish,
    vx_unpublish_kernels_f unpublish);

#ifdef __cplusplus
}
#endif

#endif /* _VX_EXT_CADENCE_H_INCLUDED_ */
