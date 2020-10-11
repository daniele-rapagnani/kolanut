let Component = import("./Component");

let Label = {
    create = |font, text| => {
        return { 
            text = text ?? "",
            color = Color.white,
            inWorldView = false,
            font = font
        } @ Label;
    },

    getType = -> {
        return "Label";
    }
} @ Component.create();

return Label;