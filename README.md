<img src="./Resources/Icon128.png" alt="Logo of the project" align="right">

# Arcstone UI / ArcUI
> Model &middot; View &middot; Presenter plugin for UE5 and UMG

This plugin uses a Model/View/Presenter architecture to separate logic and display while using UMG on UE5.

## Disclaimer

1. This plugin is experimental! So far, it is only used in personal projects.
2. This plugin relies on Epic's CommonUI plugin
3. Some parts of this plugin are inspired by Epic's Lyra project

## Installing

Clone this repository / copy the files to your Game/Plugins folder.  
Then add it to your uproject, in your declared plugins:
```json
{
    "Name": "ArcUI",
    "Enabled": true
},
```
Then in your .Build.cs, add its unique module to:
```csharp
PrivateDependencyModuleNames.Add("ArcUI");
```
And that's it! You're ready to use the plugin!


## Getting Started

TBD

### Model &middot; View &middot; Presenter: the theory

As explained on [wikipedia](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93presenter), MVP is a derivation of the MVC pattern in which the **Presenter** is the middle-man between logic and display.  
This is this philosphy that is applied is this plugin.

### ArcUI Concepts

As this is not *exactly* an MVP architecture, here are the concepts in play in this plugin:   
* **Model** - contract between a *Presenter* and a *View*. This is usually the data being sent by a *Presenter* to its View(s). Note that this plugin does not enforce anything regarding the models. It is up to the project to set them up as they wish/prefer.
* **View** - UMG class to be displayed on screen. I would recommend to define these classes in c++ and override them in Blueprint. Depending on their usage, they will be either:
  * a `UCommonUserWidget` for a classic widget
  * a `UCommonActivatableWidget` for a "main" screen like the whole HUD, a menu...
* **Presenter** - the real brains of your UI. They will listen to the game logic (events), make *Models* out of it and send them to their associated *Views*.

Additional key concepts of the plugin:
* **Asset** - the actual widgets to be loaded (the views)
* **Context** - based upon GameplayTags, contexts drive what is displayed or not. For instance you can request "ArcUI.Context.HUD" to display your whole HUD, or just request "ArcUI.Context.HUD.HealthBar" to only display the health bar from the HUD. 
* **Layers** - heavily inspired (but simplififed) from Lyra, layers define the high level positioning of your widgets.

### ArcUI Examples

TBD


## Licensing

Licensed under MIT.
