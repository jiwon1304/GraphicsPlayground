#pragma once

#define DECLARE_RENDER_PASS(RenderPassName) \
    private: \
        class F##RenderPassName##RenderPass* RenderPassName##RenderPass = nullptr; \

#define ADD_RENDER_PASS(RenderPassName) \
    RenderPassName##RenderPass = AddRenderPass<F##RenderPassName##RenderPass>();\

#define GET_RENDER_PASS(RenderPassName) \
    RenderPassName##RenderPass

