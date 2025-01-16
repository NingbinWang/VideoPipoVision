#pragma once

#include "vp_secondary_infer_node.h"
#include "rtmpose.h"

namespace vp_nodes {
    // update secondary_class_ids/secondary_labels/secondary_scores of vp_frame_target.
    class vp_rk_second_rtmpose: public vp_secondary_infer_node
    {
    private:
        /* data */
        std::shared_ptr<RTMPose> rk_model;
        vp_objects::vp_pose_type type;

    protected:
        // we need a totally new logic for the whole infer combinations
        // no separate step pre-defined needed in base class
        virtual void run_infer_combinations(const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch) override;
        // override pure virtual method, for compile pass
        virtual void postprocess(const std::vector<cv::Mat>& raw_outputs, const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& frame_meta_with_batch) override;
    public:
        vp_rk_second_rtmpose(std::string node_name, std::string config_path, std::vector<int> p_class_ids_applied_to = std::vector<int>(), int min_width_applied_to = 0, int min_height_applied_to = 0, vp_objects::vp_pose_type type=vp_objects::coco_17);
        ~vp_rk_second_rtmpose();
    };
}