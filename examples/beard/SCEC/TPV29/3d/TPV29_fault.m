clear

sim_name = 'TPV29_base_';

names = {[sim_name,'faultst-042dp061'],...
         [sim_name,'faultst-050dp000'],...
         [sim_name,'faultst-050dp100'],...
         [sim_name,'faultst-050dp160'],...
         [sim_name,'faultst-089dp101'],...
         [sim_name,'faultst-110dp014'],...
         [sim_name,'faultst-150dp050'],...
         [sim_name,'faultst-150dp120'],...
         [sim_name,'faultst-180dp156'],...
         [sim_name,'faultst000dp120' ],...
         [sim_name,'faultst043dp062' ],...
         [sim_name,'faultst046dp060' ],...
         [sim_name,'faultst050dp000' ],...
         [sim_name,'faultst050dp120' ],...
         [sim_name,'faultst051dp057' ],...
         [sim_name,'faultst059dp047' ],...
         [sim_name,'faultst090dp153' ],...
         [sim_name,'faultst093dp153' ],...
         [sim_name,'faultst100dp050' ],...
         [sim_name,'faultst100dp110' ],...
         [sim_name,'faultst150dp000' ],...
         [sim_name,'faultst150dp130' ],...
         [sim_name,'faultst167dp105' ],...
         [sim_name,'faultst170dp045' ]};

for k = 1:length(names)
  pd = process_fault_station([0,0,1],names{k},'data');

  if(isempty(pd))
    disp(['NOT FOUND :: ',names{k}])
  else
    disp(['    FOUND :: ',names{k}])
    m = [1; 0; 0];
    l = [0; 1; 0];
    t       = pd.t;
    hslip   = pd.Dp1*m(1)+pd.Dp2*m(2)+pd.Dp3*m(3);
    hrate   = pd.Vp1*m(1)+pd.Vp2*m(2)+pd.Vp3*m(3);
    hshear  = pd.Tp1*m(1)+pd.Tp2*m(2)+pd.Tp3*m(3);
    vslip   = pd.Dp1*l(1)+pd.Dp2*l(2)+pd.Dp3*l(3);
    vrate   = pd.Vp1*l(1)+pd.Vp2*l(2)+pd.Vp3*l(3);
    vshear  = pd.Tp1*l(1)+pd.Tp2*l(2)+pd.Tp3*l(3);
    nstress = pd.Tn;
    write_scec_data(['scec/',names{k},'.scec'],pd.header,...
       't h-slip h-slip-rate h-shear-stress v-slip v-slip-rate v-shear-stress n-stress ',...
       [t, hslip, hrate, hshear, vslip, vrate, vshear, nstress]);

    disp(pd.t(end))
  end
end

