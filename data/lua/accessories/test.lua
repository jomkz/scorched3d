function runWeapon(playerId, position, velocity)

	print(testvar1);

	print(playerId);
	print(position.x, position.y, position.z);
	print(velocity.x, velocity.y, velocity.z);
	
	
	tanks = s3d.get_tanks();
	for k,v in pairs(tanks) do 
		if (v.alive) then 
			print(v.name);
			print(v.id);
			print(v.position.x, v.position.y, v.position.z);
			
			t = s3d.get_tank(v.id);
			print(t.name);
			
			newvelocity = {x=0, y=0, z=0};
			
			s3dweapon.explosion(playerId, v.position, { 
				size = 10,
				hurtamount = 0,
				deform = 2,
				animate = false
			});
			
--			s3d.fire_weapon("Nuke", playerId, v.position, newvelocity);
		end
	end
	
	
	landwidth = s3d.get_landscapewidth();
	landheight = s3d.get_landscapeheight();
	
	print("Landwidth", landwidth, "Landheight", landheight);
	
	landstart = 0;
	while (landstart < landwidth and landstart < landheight) do
		height = s3d.get_height(landstart, landstart);
		s3dweapon.explosion(playerId, { x=landstart, y=landstart, z=height }, { 
			size = 4,
			hurtamount = 0,
			deform = 2,
			animate = false
		});		
		
		landstart = landstart + 15;
	end
	
	landmidx = landwidth/2;
	landmidy = landheight/2;
	
	deg = 0;
	while (deg < 3.14 * 2) do
	
		x = landmidx + math.sin(deg) * 100;
		y = landmidy + math.cos(deg) * 100;
	
		height = s3d.get_height(x, y);
		s3dweapon.explosion(playerId, { x=x, y=y, z=height }, { 
			size = 4,
			hurtamount = 0,
			deform = 2,
			animate = false
		});			
	
		deg = deg + 3.14 / 20.0;
	end
end
