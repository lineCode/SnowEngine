menuUI= class(MainUI,{
    iY = 15, 
	iX = 35,
	width = 300,
	height = 25
})

function menuUI:OnInit()
	staticText=self:AddStatic("Test",self.iY,10,600,50);
	radioButton=self:AddRadioButton("Test2",50,10,600,50);
	isChecked=false;
	listBox=self:AddListBox(15,65,600,50);
	staticText:SetText("hello!");
	listBox:AddListItem("text!");

	radioButton:RegisterCallBack(
		function()
			isChecked=not isChecked;
			radioButton:SetChecked(isChecked);
			PrintToScreen(GameOptions.m_GameName,3000);
		end
	);
end

function menuUI:OnUpdate()
	
end